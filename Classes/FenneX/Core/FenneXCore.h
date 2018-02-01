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

#ifndef FenneX_FenneXCore_h
#define FenneX_FenneXCore_h

#include "FenneXMacros.h"

#include "cocos2d.h"

//CCBIntegration
#include "CustomBaseNode.h"
#include "CustomInput.h"
#include "CustomLabel.h"
#include "CustomNode.h"
#include "CustomSprite.h"
#include "CustomScaleSprite.h"
#include "FenneXCCBLoader.h"

//Touch Handling
#include "GenericRecognizer.h"
#include "ScrollingRecognizer.h"
#include "SelectionRecognizer.h"
#include "SwipeRecognizer.h"
#include "TapRecognizer.h"
#include "TouchLinker.h"
#include "InertiaGenerator.h"
#include "Inertia.h"

//Graphics
#include "CustomObject.h"
#include "DropDownList.h"
#include "GraphicLayer.h"
#include "Image.h"
#include "InputLabel.h"
#include "InputLabelProtocol.h"
#include "LabelTTF.h"
#include "LayoutHandler.h"
#include "LazyLoader.h"
#include "Panel.h"
#include "RawObject.h"

//Scenes
#include "SceneSwitcher.h"
#include "Scene.h"

//Utility
#include "InactivityTimer.h"
#include "Localization.h"
#include "Pausable.h"
#include "PListPersist.h"
#include "RandomHelper.h"
#include "Shorteners.h"
#include "SynchronousReleaser.h"
#include "SynthesizeString.h"
#include "StringUtility.h"
#include "DelayedDispatcher.h"
#include "ValueConversion.h"

#endif
