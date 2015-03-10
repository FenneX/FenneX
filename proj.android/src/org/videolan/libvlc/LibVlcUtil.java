/*****************************************************************************
 * LibVlcUtil.java
 *****************************************************************************
 * Copyright © 2011-2013 VLC authors and VideoLAN
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/

package org.videolan.libvlc;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.io.RandomAccessFile;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.Locale;

import org.json.JSONArray;
import org.json.JSONException;

import com.fennex.modules.NativeUtility;

import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.content.SharedPreferences;
import android.net.Uri;
import android.os.Build;
import android.preference.PreferenceManager;
import android.util.Log;

public class LibVlcUtil {
    public final static String TAG = "VLC/LibVLC/Util";

    public static LibVLC getLibVlcInstance() throws LibVlcException {
        LibVLC instance = LibVLC.getExistingInstance();
        if (instance == null) {
        	Log.i("LibVlcUtil", "creating new LibVLC instance");
            Thread.setDefaultUncaughtExceptionHandler(new VlcCrashHandler());

            instance = LibVLC.getInstance();
            Context context = NativeUtility.getMainActivity().getApplicationContext();
            SharedPreferences pref = PreferenceManager.getDefaultSharedPreferences(context);
            updateLibVlcSettings(pref);
            instance.init(context);
        }
        return instance;
    }

    public static void updateLibVlcSettings(SharedPreferences pref) {
        LibVLC instance = LibVLC.getExistingInstance();
        if (instance == null)
            return;

        instance.setSubtitlesEncoding(pref.getString("subtitles_text_encoding", ""));
        instance.setTimeStretching(pref.getBoolean("enable_time_stretching_audio", true));
        instance.setFrameSkip(pref.getBoolean("enable_frame_skip", false));
        instance.setChroma(pref.getString("chroma_format", ""));
        instance.setVerboseMode(pref.getBoolean("enable_verbose_mode", true));

        if (pref.getBoolean("equalizer_enabled", false))
            instance.setEqualizer(getFloatArray(pref, "equalizer_values"));

        int aout;
        try {
            aout = Integer.parseInt(pref.getString("aout", "-1"));
        }
        catch (NumberFormatException nfe) {
            aout = -1;
        }
        int vout;
        try {
        	vout = Integer.parseInt(pref.getString("vout", "-1"));
        }
        catch (NumberFormatException nfe) {
        	vout = -1;
        }
        int deblocking;
        try {
            deblocking = Integer.parseInt(pref.getString("deblocking", "-1"));
        }
        catch(NumberFormatException nfe) {
            deblocking = -1;
        }
        int networkCaching = pref.getInt("network_caching_value", 0);
        if(networkCaching > 60000)
            networkCaching = 60000;
        else if(networkCaching < 0)
            networkCaching = 0;
        instance.setAout(aout);
        instance.setVout(vout);
        instance.setDeblocking(deblocking);
        instance.setNetworkCaching(networkCaching);
        
        String CPU_ABI = android.os.Build.CPU_ABI;
        if((CPU_ABI.equals("x86") || CPU_ABI.equals("x86_64")) && android.os.Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT)
        { //On Galaxy Tab 3 10.1" (x86), since KitKat (4.4), Hardware acceleration causes some problems. Disable it until we find a better solution
        	instance.setHardwareAcceleration(LibVLC.HW_ACCELERATION_DISABLED);        	
        }
        else
        {
        	instance.setHardwareAcceleration(LibVLC.HW_ACCELERATION_AUTOMATIC);
        }
    }

    public static float[] getFloatArray(SharedPreferences pref, String key) {
        float[] array = null;
        String s = pref.getString(key, null);
        if (s != null) {
            try {
                JSONArray json = new JSONArray(s);
                array = new float[json.length()];
                for (int i = 0; i < array.length; i++)
                    array[i] = (float) json.getDouble(i);
            } catch (JSONException e) {
                e.printStackTrace();
            }
        }
        return array;
    }
    public static boolean isFroyoOrLater()
    {
        return android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.FROYO;
    }

    public static boolean isGingerbreadOrLater()
    {
        return android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.GINGERBREAD;
    }

    public static boolean isHoneycombOrLater()
    {
        return android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.HONEYCOMB;
    }

    public static boolean isICSOrLater()
    {
        return android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.ICE_CREAM_SANDWICH;
    }

    public static boolean isJellyBeanOrLater()
    {
        return android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.JELLY_BEAN;
    }

    public static boolean isJellyBeanMR1OrLater()
    {
        return android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.JELLY_BEAN_MR1;
    }

    public static boolean isJellyBeanMR2OrLater()
    {
        return android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.JELLY_BEAN_MR2;
    }

    public static boolean isKitKatOrLater()
    {
        return android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.KITKAT;
    }

    private static String errorMsg = null;
    private static boolean isCompatible = false;
    public static String getErrorMsg() {
        return errorMsg;
    }

    public static File URItoFile(String URI) {
        if(URI == null) return null;
        return new File(Uri.decode(URI).replace("file://",""));
    }

    public static String URItoFileName(String URI) {
        if(URI == null) return null;
        return URItoFile(URI).getName();
    }

    public static boolean hasCompatibleCPU(Context context)
    {
        // If already checked return cached result
        if(errorMsg != null || isCompatible) return isCompatible;

        ApplicationInfo applicationInfo = context.getApplicationInfo();
        String libBasePath;
        if ((applicationInfo.flags & ApplicationInfo.FLAG_SYSTEM) != 0)
            libBasePath = "/system";
        else
            libBasePath = applicationInfo.dataDir;
        ElfData elf = readLib(libBasePath + "/lib/libvlcjni.so");
        if(elf == null) {
            Log.e(TAG, "WARNING: Unable to read libvlcjni.so; cannot check device ABI!");
            Log.e(TAG, "WARNING: Cannot guarantee correct ABI for this build (may crash)!");
            return true;
        }

        String CPU_ABI = android.os.Build.CPU_ABI;
        String CPU_ABI2 = "none";
        if(android.os.Build.VERSION.SDK_INT >= Build.VERSION_CODES.FROYO) { // CPU_ABI2 since 2.2
            try {
                CPU_ABI2 = (String)android.os.Build.class.getDeclaredField("CPU_ABI2").get(null);
            } catch (Exception e) { }
        }

        final boolean elfHasX86 = elf.e_machine == EM_386 || elf.e_machine == EM_X86_64;
        final boolean elfHasArm = elf.e_machine == EM_ARM || elf.e_machine == EM_AARCH64;
        final boolean elfHasMips = elf.e_machine == EM_MIPS;
        final boolean elfIs64bits = elf.is64bits;

        Log.i(TAG, "machine = " + (elfHasArm ? "arm" : elfHasX86 ? "x86" : "mips") + ", " +
                                  (elfIs64bits ? "64bits" : "32bits"));
        Log.i(TAG, "arch = " + elf.att_arch);
        Log.i(TAG, "fpu = " + elf.att_fpu);
        boolean hasNeon = false, hasFpu = false, hasArmV6 = false,
                hasArmV7 = false, hasMips = false, hasX86 = false, is64bits = false;
        float bogoMIPS = -1;
        int processors = 0;

        if(CPU_ABI.equals("x86") ||
           CPU_ABI2.equals("x86")) {
            hasX86 = true;
        } else if(CPU_ABI.equals("x86_64") ||
                  CPU_ABI2.equals("x86_64")) {
            hasX86 = true;
            is64bits = true;
        } else if(CPU_ABI.equals("armeabi-v7a") ||
                  CPU_ABI2.equals("armeabi-v7a")) {
            hasArmV7 = true;
            hasArmV6 = true; /* Armv7 is backwards compatible to < v6 */
        } else if(CPU_ABI.equals("armeabi") ||
                  CPU_ABI2.equals("armeabi")) {
            hasArmV6 = true;
        } else if(CPU_ABI.equals("arm64-v8a") ||
                CPU_ABI2.equals("arm64-v8a")) {
            hasNeon = true;
            hasArmV6 = true;
            hasArmV7 = true;
            is64bits = true;
        }

        try {
            FileReader fileReader = new FileReader("/proc/cpuinfo");
            BufferedReader br = new BufferedReader(fileReader);
            String line;
            while((line = br.readLine()) != null) {
                if(!hasArmV7 && line.contains("AArch64")) {
                    hasArmV7 = true;
                    hasArmV6 = true; /* Armv8 is backwards compatible to < v7 */
                }
                if(!hasArmV7 && line.contains("ARMv7")) {
                    hasArmV7 = true;
                    hasArmV6 = true; /* Armv7 is backwards compatible to < v6 */
                }
                if(!hasArmV7 && !hasArmV6 && line.contains("ARMv6"))
                    hasArmV6 = true;
                // "clflush size" is a x86-specific cpuinfo tag.
                // (see kernel sources arch/x86/kernel/cpu/proc.c)
                if(line.contains("clflush size"))
                    hasX86 = true;
                if(line.contains("GenuineIntel"))
                    hasX86 = true;
                // "microsecond timers" is specific to MIPS.
                // see arch/mips/kernel/proc.c
                if(line.contains("microsecond timers"))
                    hasMips = true;
                if(!hasNeon && (line.contains("neon") || line.contains("asimd")))
                    hasNeon = true;
                if(!hasFpu && (line.contains("vfp") || (line.contains("Features") && line.contains("fp"))))
                    hasFpu = true;
                if(line.startsWith("processor"))
                    processors++;
                if(bogoMIPS < 0 && line.toLowerCase(Locale.ENGLISH).contains("bogomips")) {
                    String[] bogo_parts = line.split(":");
                    try {
                        bogoMIPS = Float.parseFloat(bogo_parts[1].trim());
                    } catch(NumberFormatException e) {
                        bogoMIPS = -1; // invalid bogomips
                    }
                }
            }
            fileReader.close();
        } catch(IOException ex){
            ex.printStackTrace();
            errorMsg = "IOException whilst reading cpuinfo flags";
            isCompatible = false;
            return false;
        }
        if(processors == 0)
            processors = 1; // possibly borked cpuinfo?

        // Enforce proper architecture to prevent problems
        if(elfHasX86 && !hasX86) {
            errorMsg = "x86 build on non-x86 device";
            isCompatible = false;
            return false;
        } else if(elfHasArm && hasX86) {
            errorMsg = "ARM build on x86 device";
            isCompatible = false;
            return false;
        }

        if(elfHasMips && !hasMips) {
            errorMsg = "MIPS build on non-MIPS device";
            isCompatible = false;
            return false;
        } else if(elfHasArm && hasMips) {
            errorMsg = "ARM build on MIPS device";
            isCompatible = false;
            return false;
        }

        if(elf.e_machine == EM_ARM && elf.att_arch.startsWith("v7") && !hasArmV7) {
            errorMsg = "ARMv7 build on non-ARMv7 device";
            isCompatible = false;
            return false;
        }
        if(elf.e_machine == EM_ARM) {
            if(elf.att_arch.startsWith("v6") && !hasArmV6) {
                errorMsg = "ARMv6 build on non-ARMv6 device";
                isCompatible = false;
                return false;
            } else if(elf.att_fpu && !hasFpu) {
                errorMsg = "FPU-enabled build on non-FPU device";
                isCompatible = false;
                return false;
            }
        }
        if (elfIs64bits && !is64bits) {
            errorMsg = "64bits build on 32bits device";
            isCompatible = false;
        }

        float frequency = -1;
        try {
            FileReader fileReader = new FileReader("/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq");
            BufferedReader br = new BufferedReader(fileReader);
            String line = "";
            try {
                line = br.readLine();
                frequency = Float.parseFloat(line) / 1000.f; /* Convert to MHz */
            } catch(NumberFormatException e) {
                Log.w(TAG, "Could not parse maximum CPU frequency!");
                Log.w(TAG, "Failed to parse: " + line);
            }
            fileReader.close();
        } catch(IOException ex) {
            Log.w(TAG, "Could not find maximum CPU frequency!");
        }

        errorMsg = null;
        isCompatible = true;
        // Store into MachineSpecs
        machineSpecs = new MachineSpecs();
        machineSpecs.hasArmV6 = hasArmV6;
        machineSpecs.hasArmV7 = hasArmV7;
        machineSpecs.hasFpu = hasFpu;
        machineSpecs.hasMips = hasMips;
        machineSpecs.hasNeon = hasNeon;
        machineSpecs.hasX86 = hasX86;
        machineSpecs.is64bits = is64bits;
        machineSpecs.bogoMIPS = bogoMIPS;
        machineSpecs.processors = processors;
        machineSpecs.frequency = frequency;
        return true;
    }

    public static MachineSpecs getMachineSpecs() {
        return machineSpecs;
    }
    private static MachineSpecs machineSpecs = null;
    public static class MachineSpecs {
        public boolean hasNeon;
        public boolean hasFpu;
        public boolean hasArmV6;
        public boolean hasArmV7;
        public boolean hasMips;
        public boolean hasX86;
        public boolean is64bits;
        public float bogoMIPS;
        public int processors;
        public float frequency; /* in MHz */
    }

    private static final int EM_386 = 3;
    private static final int EM_MIPS = 8;
    private static final int EM_ARM = 40;
    private static final int EM_X86_64 = 62;
    private static final int EM_AARCH64 = 183;
    private static final int ELF_HEADER_SIZE = 52;
    private static final int SECTION_HEADER_SIZE = 40;
    private static final int SHT_ARM_ATTRIBUTES = 0x70000003;
    private static class ElfData {
        ByteOrder order;
        boolean is64bits;
        int e_machine;
        int e_shoff;
        int e_shnum;
        int sh_offset;
        int sh_size;
        String att_arch;
        boolean att_fpu;
    }

    /** '*' prefix means it's unsupported */
    private static String[] CPU_archs = {"*Pre-v4", "*v4", "*v4T",
                                         "v5T", "v5TE", "v5TEJ",
                                         "v6", "v6KZ", "v6T2", "v6K", "v7",
                                         "*v6-M", "*v6S-M", "*v7E-M", "*v8"};

    private static ElfData readLib(String path) {
        File file = new File(path);
        if (!file.exists() || !file.canRead())
            return null;

        RandomAccessFile in = null;
        try {
            in = new RandomAccessFile(file, "r");

            ElfData elf = new ElfData();
            if (!readHeader(in, elf))
                return null;

            switch (elf.e_machine) {
                case EM_386:
                case EM_MIPS:
                case EM_X86_64:
                case EM_AARCH64:
                    return elf;
                case EM_ARM:
                    in.close();
                    in = new RandomAccessFile(file, "r");
                    if (!readSection(in, elf))
                        return null;
                    in.close();
                    in = new RandomAccessFile(file, "r");
                    if (!readArmAttributes(in, elf))
                        return null;
                    break;
                default:
                    return null;
            }
            return elf;
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            try {
                if (in != null)
                    in.close();
            } catch (IOException e) {
            }
        }
        return null;
    }

    private static boolean readHeader(RandomAccessFile in, ElfData elf) throws IOException {
        // http://www.sco.com/developers/gabi/1998-04-29/ch4.eheader.html
        byte[] bytes = new byte[ELF_HEADER_SIZE];
        in.readFully(bytes);
        if (bytes[0] != 127 ||
                bytes[1] != 'E' ||
                bytes[2] != 'L' ||
                bytes[3] != 'F' ||
                (bytes[4] != 1 && bytes[4] != 2)) {
            Log.e(TAG, "ELF header invalid");
            return false;
        }

        elf.is64bits = bytes[4] == 2;
        elf.order = bytes[5] == 1
                ? ByteOrder.LITTLE_ENDIAN // ELFDATA2LSB
                : ByteOrder.BIG_ENDIAN;   // ELFDATA2MSB

        // wrap bytes in a ByteBuffer to force endianess
        ByteBuffer buffer = ByteBuffer.wrap(bytes);
        buffer.order(elf.order);

        elf.e_machine = buffer.getShort(18);    /* Architecture */
        elf.e_shoff = buffer.getInt(32);        /* Section header table file offset */
        elf.e_shnum = buffer.getShort(48);      /* Section header table entry count */
        return true;
    }

    private static boolean readSection(RandomAccessFile in, ElfData elf) throws IOException {
        byte[] bytes = new byte[SECTION_HEADER_SIZE];
        in.seek(elf.e_shoff);

        for (int i = 0; i < elf.e_shnum; ++i) {
            in.readFully(bytes);

            // wrap bytes in a ByteBuffer to force endianess
            ByteBuffer buffer = ByteBuffer.wrap(bytes);
            buffer.order(elf.order);

            int sh_type = buffer.getInt(4); /* Section type */
            if (sh_type != SHT_ARM_ATTRIBUTES)
                continue;

            elf.sh_offset = buffer.getInt(16);  /* Section file offset */
            elf.sh_size = buffer.getInt(20);    /* Section size in bytes */
            return true;
        }

        return false;
    }

    private static boolean readArmAttributes(RandomAccessFile in, ElfData elf) throws IOException {
        byte[] bytes = new byte[elf.sh_size];
        in.seek(elf.sh_offset);
        in.readFully(bytes);

        // wrap bytes in a ByteBuffer to force endianess
        ByteBuffer buffer = ByteBuffer.wrap(bytes);
        buffer.order(elf.order);

        //http://infocenter.arm.com/help/topic/com.arm.doc.ihi0044e/IHI0044E_aaelf.pdf
        //http://infocenter.arm.com/help/topic/com.arm.doc.ihi0045d/IHI0045D_ABI_addenda.pdf
        if (buffer.get() != 'A') // format-version
            return false;

        // sub-sections loop
        while (buffer.remaining() > 0) {
            int start_section = buffer.position();
            int length = buffer.getInt();
            String vendor = getString(buffer);
            if (vendor.equals("aeabi")) {
                // tags loop
                while (buffer.position() < start_section + length) {
                    int start = buffer.position();
                    int tag = buffer.get();
                    int size = buffer.getInt();
                    // skip if not Tag_File, we don't care about others
                    if (tag != 1) {
                        buffer.position(start + size);
                        continue;
                    }

                    // attributes loop
                    while (buffer.position() < start + size) {
                        tag = getUleb128(buffer);
                        if (tag == 6) { // CPU_arch
                            int arch = getUleb128(buffer);
                            elf.att_arch = CPU_archs[arch];
                        }
                        else if (tag == 27) { // ABI_HardFP_use
                            getUleb128(buffer);
                            elf.att_fpu = true;
                        }
                        else {
                            // string for 4=CPU_raw_name / 5=CPU_name / 32=compatibility
                            // string for >32 && odd tags
                            // uleb128 for other
                            tag %= 128;
                            if (tag == 4 || tag == 5 || tag == 32 || (tag > 32 && (tag & 1) != 0))
                                getString(buffer);
                            else
                                getUleb128(buffer);
                        }
                    }
                }
                break;
            }
        }
        return true;
    }

    private static String getString(ByteBuffer buffer) {
        StringBuilder sb = new StringBuilder(buffer.limit());
        while (buffer.remaining() > 0) {
            char c = (char) buffer.get();
            if (c == 0)
                break;
            sb.append(c);
        }
        return sb.toString();
    }

    private static int getUleb128(ByteBuffer buffer) {
        int ret = 0;
        int c;
        do {
            ret <<= 7;
            c = buffer.get();
            ret |= c & 0x7f;
        } while((c & 0x80) > 0);

        return ret;
    }
}
