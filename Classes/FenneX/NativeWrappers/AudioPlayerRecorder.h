/****************************************************************************
Copyright (c) 2013-2014 Auticiel SAS

http://www.fennex.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************///

#ifndef FenneX_AudioPlayerRecorder_h
#define FenneX_AudioPlayerRecorder_h

#include "FenneX.h"
#include "Shorteners.h"
USING_NS_CC;
USING_NS_FENNEX;

#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
#define SOUND_EXTENSION ".caf"
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
#define SOUND_EXTENSION ".3gp"
#endif

#define INFINITE_PLAY -1

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
//use native calls for play/record. The extension is left to the implementation
//currently record as .caf on iOS and as .3gp on Android
//TODO : unify as only 1 audio format (wav for example)
//Warning : incompatible with VideoRecorder (picking video from camera) on iOS
/* If the app crash on record on Android, check that you have those lines in your AndroidManifest.xml :
	<uses-permission android:name="android.permission.RECORD_AUDIO" />
	<uses-feature android:name="android.hardware.microphone"/>
 */
class AudioPlayerRecorder : public Ref
{
public:
    //shared methods
    static AudioPlayerRecorder* sharedRecorder(void);
    ~AudioPlayerRecorder();
    
    static float getSoundDuration(std::string file);
    static std::string getSoundsSavePath();
    
    //By default, recording is disabled (to avoid prompting for microphone on iOS)
    //You should enable it when the user goes to somewhere he is going to need recording soon, then disable it once he leaves the area
    //When disabled, iOS use the AVAudioSessionCategorySoloAmbient mode
    //When enabled, iOS use the AVAudioSessionCategoryPlayAndRecord mode
    //On Android, this method have no effect, since
    void setRecordEnabled(bool enabled);
    bool isRecordEnabled();
    
    Ref* getLink();
    std::string getPath();
    std::string getPathWithoutExtension();
    void stopAll();
    
    //Those methods first try Object before Sender because it can be a subcall (for example if the selection is via a pop-up. If the Sender is a CCInteger, it will try to get a valid RawObject from GraphicLayer
    void playObject(EventCustom* event); //infos may contains the Object/Sender (Ref*) and the File (CCString)
    void recordObject(EventCustom* event); //infos may contains the Object/Sender (Ref*), the File (CCString) will be generated if absent
    
    //implementation specific methods
    bool isRecording();
    bool isPlaying();
    
    void record(const std::string&  file, Ref* linkTo);
    void stopRecording();
    float play(const std::string&  file, Ref* linkTo, bool independent = false); //return the duration of the file
    void stopPlaying(EventCustom* event = NULL);
    void fadeVolumeOut();
    
    void play();
    void pause();
    void restart();
    
    //deleteFile requires the full path including the extension
    void deleteFile(const std::string& file);
    
    void setNumberOfLoops(int loops);
    
    /* Get the file metadata, if available :
        - Author (CCString)
        - Title (CCString)
        - Duration (CCInteger, as seconds)
     Anything can be NULL, including the returned Dictionary if something went wrong or not implemented
     */
    static CCDictionary* getFileMetadata(const std::string& path);
protected:
    AudioPlayerRecorder();
    void init();
    Ref* link; //the object that required the record/play
    std::string path; //the current path being recorded/played
    void setPath(std::string value);
    void setLink(Ref* value);
    bool recordEnabled;
    
    Ref* noLinkObject; //If a sound have no link, it is linked to this object so that everything works well
    Vector<EventListenerCustom*> listeners;
};
#endif

static inline void notifyPlayingSoundEnded()
{
    DelayedDispatcher::eventAfterDelay("PlayingSoundEnded", Dcreate(), 0.01);
}

#endif
