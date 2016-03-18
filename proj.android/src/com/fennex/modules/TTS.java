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

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Locale;

import android.annotation.SuppressLint;
import android.os.Build;
import android.provider.Settings;
import android.speech.tts.TextToSpeech;
import android.speech.tts.TextToSpeech.OnUtteranceCompletedListener;
import android.speech.tts.UtteranceProgressListener;
import android.util.Log;

@SuppressWarnings("deprecation")
public class TTS implements TextToSpeech.OnInitListener
{
	static private TTS instance = null;
	private TextToSpeech engine;
	private boolean isInit;
	private boolean available;
	private float desiredRate;
	private ArrayList<String> preinitQueue;
	HashMap<String, String> settings;
	
    public native static void onTTSEnd();
	
	static TTS getInstance()
	{
		if(instance == null)
		{
			instance = new TTS();
		}
		return instance;
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
		preinitQueue = new ArrayList<String>();
		settings = new HashMap<String, String>();
		desiredRate = 1.0f;
		if(android.os.Build.VERSION.SDK_INT >= 15 )
		{
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
		else
		{
			engine.setOnUtteranceCompletedListener(new OnUtteranceCompletedListener()
			{

				@Override
				public void onUtteranceCompleted(String utteranceId)
				{
					onTTSEnd();
				}
			
			});
		}
	}

	@Override
	public void onInit(int status) {
		isInit = true;
		available = status == TextToSpeech.SUCCESS;
		if(available && preinitQueue != null && engine != null) {
			for(String s : preinitQueue)
			{	
				if(s == preinitQueue.get(preinitQueue.size()-1))
				{
					engine.speak(s, TextToSpeech.QUEUE_ADD, settings);
				}
                else if(s == preinitQueue.get(0))
                {
                    engine.speak(s, TextToSpeech.QUEUE_FLUSH, null);
                }
				else
				{
					engine.speak(s, TextToSpeech.QUEUE_ADD, null);
				}
				engine.speak(s, TextToSpeech.QUEUE_ADD, settings);
			}
			preinitQueue.clear();
		}
		else
		{
			Log.i("TTS", "TTS unavailable, init failed");
		}
	}
	
	public void notifyTTSEnd()
	{
		onTTSEnd();
	}
	
	public boolean speakText(String[] text) {
		if(isInit && available && engine != null) {
			for(String item : text)
			{

				if(item == text[text.length-1])
				{
					settings.put(TextToSpeech.Engine.KEY_PARAM_UTTERANCE_ID,"messageID");
					engine.speak(item, TextToSpeech.QUEUE_ADD, settings);
				}
				else
				{
					engine.speak(item, TextToSpeech.QUEUE_ADD, null);
					engine.playSilence(1000, TextToSpeech.QUEUE_ADD, null);
				}
					
			}
			return true;
		}
		else if(!isInit)
		{
			for(String item : text)
			{
				preinitQueue.add(item);		
			}
			
			return true;
		}
		Log.i("TTS", "TTS unavailable, init failed");
		return false;
	}
	
	public boolean isSpeaking() {
		if(isInit && available && engine != null) {
            return engine.isSpeaking();
        }
        return false;
	}

	public void stopSpeakText() {
		if(isInit && available && engine != null) {
			engine.stop();
		}
	}

	public String getTTSEngineName()
	{
		if(engine != null)
		{
			return engine.getDefaultEngine();
		}
		return "android.tts.engine";
	}

	public float getTTSPlayRate()
	{
		return desiredRate;
	}

	public void setTTSPlayRate(float rate)
	{
		desiredRate = rate;
		if(engine != null)
		{
			engine.setSpeechRate(0.75f * desiredRate);
		}
	}
}