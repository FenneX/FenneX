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

#ifndef __FenneX__FenneXCCBLoader__
#define __FenneX__FenneXCCBLoader__

#include "cocos2d.h"
#include "FenneXMacros.h"
#include "editor-support/cocosbuilder/CocosBuilder.h"

USING_NS_CC;

#include "Panel.h"

NS_FENNEX_BEGIN

/*
 Every text loaded (for Labels and InputLabels) will be passed through this function.
 Useful for translation purposes, or any other text transformation you need to do during CCB loading.
 Function parameters:
 - ccb file name passed to loadCCBFromFile
 - text to transform
 - object parameters
*/
void setCCBLoadingTextTransform(std::function<std::string(const std::string&, const std::string&, const ValueMap&)> _textTransform);

/* This method is used to load a hierarchy from a .ccbi file into into FenneX. The file must not have a .ccbi extension
 Specify a panel if you want the file to be loaded in a panel rather than as GraphicLayer base node (for a popup for example)
 Note : Label in CocosBuilder must be added in the same resolution as DesignResolution (from AppMacros.h)
 
 Refer to loadNodeToFenneX for more info on the mapping
 */
Panel* loadCCBFromFileToFenneX(std::string file, std::string inPanel = "", int zIndex = 0);

/* This method is used to load a hierarchy loaded by CCBReader into FenneX.
 It is used recursively by Panels to create their own hierarchy. The base node (when parent == nullptr) will be used as GraphicLayer base node
 Note : the base node won't be loaded and must not be modified in cocos builder
 The mapping is :
 Sprite => CustomSprite => Image
 Node => CustomNode => Panel
 Label => CustomLabel => LabelTTF
 ui::Scale9Sprite => CustomInput => InputLabel
 
 CustomObject, if required, could be a derived Node
 */
void loadNodeToFenneX(std::string file, Node* baseNode, Panel* parent = nullptr);
//It is required to do another pass after loadNodeToFenneX to reorder Zindex (especially because of InputLabel which are created and added at the end instead of the right place)
void reorderZindex();
//It is required to do another pass after loadNodeToFenneX to have the input labels linked to their LabelTTF
void linkInputLabels();

void CCBLoaderSetScale(float scale);
float CCBLoaderGetScale();

//The size in which the CCB will be loaded. By default, it's the size of the screen.
//You might want to make it smaller to leave some space at the top of the screen for a banner.
void CCBLoaderSetLoadSize(cocos2d::Size loadSize);
cocos2d::Size CCBLoaderGetLoadSize();

void CCBLoaderSetPhoneLayout(bool usePhone);
bool CCBLoaderIsPhoneLayout();

std::vector<cocosbuilder::CCBAnimationManager*>& getAnimationManagers();
NS_FENNEX_END

#endif /* defined(__FenneX__FenneXCCBLoader__) */
