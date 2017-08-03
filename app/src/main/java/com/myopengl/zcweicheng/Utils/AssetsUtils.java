package com.myopengl.zcweicheng.Utils;

import com.myopengl.zcweicheng.MyApp;

import java.io.BufferedReader;
import java.io.InputStreamReader;

/**
 * Created by zhengweicheng on 2017/8/2 0002.
 */

public class AssetsUtils {

    public static String getFromAssets(String fileName){
        try {
            InputStreamReader inputReader = new InputStreamReader( MyApp.getContext().getAssets().open(fileName) );
            BufferedReader bufReader = new BufferedReader(inputReader);
            String line;
            final StringBuilder body = new StringBuilder();
            while((line = bufReader.readLine()) != null) {
                body.append(line);
                body.append("\n");
            }


            return body.toString();
        } catch (Exception e) {
            e.printStackTrace();
        }
        return null;
    }
}
