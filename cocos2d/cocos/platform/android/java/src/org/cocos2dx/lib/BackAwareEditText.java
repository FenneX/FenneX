package org.cocos2dx.lib;

import android.content.Context;
import android.util.AttributeSet;
import android.view.KeyEvent;
import android.widget.EditText;

public class BackAwareEditText extends EditText {

	public BackAwareEditText(Context context) {
		super(context);
		// TODO Auto-generated constructor stub
	}

	public BackAwareEditText(Context context, AttributeSet attrs) {
		super(context, attrs);
		// TODO Auto-generated constructor stub
	}

	public BackAwareEditText(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
		// TODO Auto-generated constructor stub
	}
	
	//On back, return a backspace so that CCEditBoxImplAndroid know the text shouldn't be changed
	public boolean onKeyDown(final int pKeyCode, final KeyEvent pKeyEvent) {
		super.onKeyDown(pKeyCode, pKeyEvent);

		/* Let GlSurfaceView get focus if back key is input. */
		if (pKeyCode == KeyEvent.KEYCODE_BACK) {
   			Cocos2dxHelper.setEditTextDialogResult("\b");
		}

		return false;
	}
}
