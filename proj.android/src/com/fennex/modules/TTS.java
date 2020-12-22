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

package com.fennex.modules;

import android.annotation.SuppressLint;
import android.speech.tts.TextToSpeech;
import android.speech.tts.UtteranceProgressListener;
import android.util.Log;

import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Locale;

@SuppressWarnings({"unused", "FieldMayBeFinal"})
public class TTS implements TextToSpeech.OnInitListener
{
	static private TTS instance = null;
	private TextToSpeech engine;
	private boolean isInit;
	private boolean available;
	private float desiredRate;
	private String engineName;
	private ArrayList<String> preinitQueue;
	private HashMap<String, String> settings;

	public native static void onTTSEnd();

	static TTS getInstance()
	{
		if(instance == null)
		{
			instance = new TTS();
		}
		return instance;
	}

	static void initInstance(String engineName)
	{
		if(instance == null)
		{
			instance = new TTS(engineName);
		}
	}

	@SuppressLint("NewApi")
	private TTS()
	{
		isInit = false;
		available = false;
		engine = new TextToSpeech(NativeUtility.getMainActivity(), this);
		engine.setLanguage(Locale.FRANCE);
		engine.setPitch(1.0f);
		engine.setSpeechRate(0.75f);
		engineName = getTTSDefaultEngineName();
		preinitQueue = new ArrayList<>();
		settings = new HashMap<>();
		desiredRate = 1.0f;
		engine.setOnUtteranceProgressListener(new UtteranceProgressListener()
		{
			@Override
			public void onStart(String utteranceId)
			{
				//do nothing
			}

			@Override
			public void onError(String utteranceId)
			{
				onTTSEnd();
			}

			@Override
			public void onDone(String utteranceId)
			{
				onTTSEnd();
			}
		});
	}
	@SuppressLint("NewApi")
	private TTS(String _engineName)
	{
		isInit = false;
		available = false;
		engineName = _engineName;
		engine = new TextToSpeech(NativeUtility.getMainActivity(), this, engineName);
		engine.setLanguage(Locale.FRANCE);
		engine.setPitch(1.0f);
		engine.setSpeechRate(0.75f);
		preinitQueue = new ArrayList<>();
		settings = new HashMap<>();
		desiredRate = 1.0f;
		engine.setOnUtteranceProgressListener(new UtteranceProgressListener()
		{
			@Override
			public void onStart(String utteranceId)
			{
				//do nothing
			}

			@Override
			public void onError(String utteranceId)
			{
				onTTSEnd();
			}

			@Override
			public void onDone(String utteranceId)
			{
				onTTSEnd();
			}
		});
	}

	@Override
	public void onInit(int status) {
		isInit = true;
		available = status == TextToSpeech.SUCCESS;
		if(available && preinitQueue != null && engine != null) {
			for(int i = 0; i < preinitQueue.size(); i++) {
				String s = preinitQueue.get(i);
				if(i == preinitQueue.size()-1) {
					//Only add settings (which contains Utterance ID) in last text
					engine.speak(s, TextToSpeech.QUEUE_ADD, settings);
				}
				else if(i == 0) {
					//First text flush the queue, just in case: TTS is not supposed to be initialized before
					engine.speak(s, TextToSpeech.QUEUE_FLUSH, null);
				}
				else {
					//Anything in-between is just added normally to the queue
					engine.speak(s, TextToSpeech.QUEUE_ADD, null);
				}
			}
			preinitQueue.clear();
		}
		else
		{
			Log.i("TTS", "TTS unavailable, init failed");
		}
	}

	@SuppressWarnings("unused")
	public void notifyTTSEnd()
	{
		onTTSEnd();
	}

	@SuppressWarnings("unused")
	public boolean speakText(String[] text, float volume) {
		if(isInit && available && engine != null) {
			try {
				for (int i = 0; i < text.length; i++) {
					String item = text[i];
					if (i == text.length - 1) {
						settings.put(TextToSpeech.Engine.KEY_PARAM_UTTERANCE_ID, "messageID");
						settings.put(TextToSpeech.Engine.KEY_PARAM_VOLUME, "" + volume);
						engine.speak(item, TextToSpeech.QUEUE_ADD, settings);
					} else {
						engine.speak(item, TextToSpeech.QUEUE_ADD, null);
						engine.playSilence(1000, TextToSpeech.QUEUE_ADD, null);
					}
				}
				return true;
			} catch (IllegalArgumentException e) {
				//Bug happening in the wild, despite waiting for engine initialization.
				//For now, assume the service isn't actually registered yet
				if(e.getMessage() != null && e.getMessage().contains("Service not registered")) {
					Collections.addAll(preinitQueue, text);
					return true;
				}
				//Something else failed that we didn't think about, re-throw
				throw e;
			}
		}
		else if(!isInit)
		{
			Collections.addAll(preinitQueue, text);
			return true;
		}
		Log.i("TTS", "TTS unavailable, init failed");
		return false;
	}

	@SuppressWarnings("unused")
	public boolean isSpeaking() {
		if(isInit && available && engine != null) {
			return engine.isSpeaking();
		}
		return false;
	}

	@SuppressWarnings("unused")
	public void stopSpeakText() {
		if(isInit && available && engine != null) {
			engine.stop();
		}
	}

	@SuppressWarnings("unused")
	public String getTTSEngineName()
	{
		if(!engineName.isEmpty())
		{
			return engineName;
		}
		return "android.tts.engine";
	}

	@SuppressWarnings("unused")
	public String[][] getTTSEngines()
	{
		if(engine != null)
		{
			List<String[]> toReturn = new ArrayList<>();
			for(TextToSpeech.EngineInfo currentEngine : engine.getEngines())
			{
				toReturn.add(new String[]{currentEngine.name, currentEngine.label});
			}
			String[][] tab = new String[toReturn.size()][2];
			tab = toReturn.toArray(tab);
			return tab;
		}
		return new String[][]{{"android.tts.engine", ""}};
	}

	@SuppressWarnings("unused")
	public static void setTTSEngine(String ttsEngine)
	{
		if(!getInstance().engineName.equals(ttsEngine))
		{
			float rate = getInstance().getTTSPlayRate();
			instance = new TTS(ttsEngine);
			instance.setTTSPlayRate(rate);
		}
	}

	@SuppressWarnings("WeakerAccess")
	public float getTTSPlayRate()
	{
		return desiredRate;
	}

	@SuppressWarnings("WeakerAccess")
	public void setTTSPlayRate(float rate)
	{
		desiredRate = rate;
		if(engine != null)
		{
			engine.setSpeechRate(0.75f * desiredRate);
		}
	}

	private String getTTSDefaultEngineName()
	{
		if(engine != null)
		{
			return engine.getDefaultEngine();
		}
		return "android.tts.engine";
	}
}