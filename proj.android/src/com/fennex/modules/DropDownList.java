package com.fennex.modules;

import android.app.AlertDialog;
import android.content.DialogInterface;

/**
 * Created by admin on 23/09/15.
 */
public class DropDownList {

    private String[]  possibleValues;
    private String title;
    private int identifier;

    static public DropDownList create()
    {
        return new DropDownList();
    }

    public void DropDownList()
    {
        title = "";
        identifier = -1;
        possibleValues = new String[]{};

    }

    public void show()
    {
        if(NativeUtility.getMainActivity() != null)
        {
            NativeUtility.getMainActivity().runOnUiThread(new Runnable() {
                public void run() {
                    AlertDialog.Builder builder = new AlertDialog.Builder(NativeUtility.getMainActivity());
                    builder.setTitle(title);
                    builder.setItems(possibleValues, new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialog, int which) {
                            notifySelectionDone(identifier, possibleValues[which]);
                            dialog.dismiss();
                        }
                    });
                    builder.show();
                }
            });
        }
    }

    public void setPossibleValues(String[] newPossibleValues)
    {
        possibleValues = newPossibleValues;
    }

    public void setIdentifier(int _identifier)
    {
        identifier = _identifier;
    }

    public void setTitle(String newtitle)
    {
        title = newtitle;
    }

    private native static void notifySelectionDone(int identifier, String value);
}
