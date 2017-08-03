package com.myopengl.zcweicheng.ui;

import android.app.Activity;
import android.graphics.Matrix;
import android.graphics.SurfaceTexture;
import android.os.Bundle;
import android.util.Log;
import android.view.Surface;
import android.view.TextureView;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.Spinner;

import com.myopengl.zcweicheng.R;
import com.myopengl.zcweicheng.gles.MiscUtils;
import com.myopengl.zcweicheng.gles.MoviePlayer;
import com.myopengl.zcweicheng.gles.MoviePlayer.PlayerFeedback;
import com.myopengl.zcweicheng.gles.SpeedControlCallback;
import com.myopengl.zcweicheng.render.VideoTextureRender;
import com.myopengl.zcweicheng.render.VideoTextureRender.VideoTextureRenderListener;

import java.io.File;
import java.io.IOException;

/**
 * Created by zhengweicheng on 2017/8/1 0001.
 */

public class PlayVideoActivity extends Activity implements AdapterView.OnItemSelectedListener,
        TextureView.SurfaceTextureListener, PlayerFeedback {

    private static final String TAG = "PlayVideoActivity";
    private TextureView mTextureView;
    private String[] mMovieFiles;
    private int mSelectedMovie;
    private boolean mShowStopLabel;
    private boolean mSurfaceTextureReady;
    private VideoTextureRender mVideoTextureRender;
    private MoviePlayer.PlayTask mPlayTask;
    private Surface mInputSurface;

    @Override
    protected void onPause() {
        Log.d(TAG, "PlayMovieActivity onPause");
        super.onPause();
        if (mPlayTask != null) {
            stopPlayback();
            mPlayTask.waitForStop();
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_play_movie);

        mTextureView = (TextureView) findViewById(R.id.movie_texture_view);
        mTextureView.setSurfaceTextureListener(this);
        // Populate file-selection spinner.
        Spinner spinner = (Spinner) findViewById(R.id.playMovieFile_spinner);
        // Need to create one of these fancy ArrayAdapter thingies, and specify the generic layout
        // for the widget itself.
        mMovieFiles = MiscUtils.getFiles(getFilesDir(), "*.mp4");
        ArrayAdapter<String> adapter = new ArrayAdapter<String>(this,
                android.R.layout.simple_spinner_item, mMovieFiles);
        adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        // Apply the adapter to the spinner.
        spinner.setAdapter(adapter);
        spinner.setOnItemSelectedListener(this);
    }

    @Override
    public void onItemSelected(AdapterView<?> parent, View view, int pos, long id) {
        Spinner spinner = (Spinner) parent;
        mSelectedMovie = spinner.getSelectedItemPosition();

        Log.d(TAG, "onItemSelected: " + mSelectedMovie + " '" + mMovieFiles[mSelectedMovie] + "'");
    }

    @Override public void onNothingSelected(AdapterView<?> parent) {}

    @Override
    public void onSurfaceTextureAvailable(SurfaceTexture surface, int width, int height) {
        mVideoTextureRender = new VideoTextureRender(new Surface(surface), width, height, new VideoTextureRenderListener() {
            @Override
            public void onInputTextureCreate(SurfaceTexture surfaceTexture, int width, int height) {
                mInputSurface = new Surface(surfaceTexture);
                mSurfaceTextureReady = true;
            }
        });
    }

    @Override
    public void onSurfaceTextureSizeChanged(SurfaceTexture surface, int width, int height) {

    }

    @Override
    public boolean onSurfaceTextureDestroyed(SurfaceTexture surface) {
        if (mVideoTextureRender != null) {
            mVideoTextureRender.release();
        }
        return false;
    }

    @Override
    public void onSurfaceTextureUpdated(SurfaceTexture surface) {

    }

    /**
     * onClick handler for "play"/"stop" button.
     */
    public void clickPlayStop(@SuppressWarnings("unused") View unused) {
        if (mInputSurface == null) {
            return;
        }
        if (mShowStopLabel) {
            Log.d(TAG, "stopping movie");
            stopPlayback();
            // Don't update the controls here -- let the task thread do it after the movie has
            // actually stopped.
            //mShowStopLabel = false;
            //updateControls();
        } else {
            if (mPlayTask != null) {
                Log.w(TAG, "movie already playing");
                return;
            }
            Log.d(TAG, "starting movie");
            SpeedControlCallback callback = new SpeedControlCallback();
            if (((CheckBox) findViewById(R.id.locked60fps_checkbox)).isChecked()) {
                // TODO: consider changing this to be "free running" mode
                callback.setFixedPlaybackRate(60);
            }
//            SurfaceTexture st = mTextureView.getSurfaceTexture();
//            st.setOnFrameAvailableListener(new SurfaceTexture.OnFrameAvailableListener() {
//                @Override
//                public void onFrameAvailable(SurfaceTexture surfaceTexture) {
//                    surfaceTexture.updateTexImage();
//                    Log.d(TAG, "onFrameAvailable");
//                }
//            });
            MoviePlayer player = null;
            try {
                player = new MoviePlayer(new File(getFilesDir(), mMovieFiles[mSelectedMovie]), mInputSurface, callback);
            } catch (IOException ioe) {
                Log.e(TAG, "Unable to play movie", ioe);
                mInputSurface.release();
                return;
            }
            adjustAspectRatio(player.getVideoWidth(), player.getVideoHeight());

            mPlayTask = new MoviePlayer.PlayTask(player, this);
            if (((CheckBox) findViewById(R.id.loopPlayback_checkbox)).isChecked()) {
                mPlayTask.setLoopMode(true);
            }

            mShowStopLabel = true;
            updateControls();
            mPlayTask.execute();
        }
    }

    /**
     * Requests stoppage if a movie is currently playing.  Does not wait for it to stop.
     */
    private void stopPlayback() {
        if (mPlayTask != null) {
            mPlayTask.requestStop();
        }
    }

    @Override   // MoviePlayer.PlayerFeedback
    public void playbackStopped() {
        Log.d(TAG, "playback stopped");
        mShowStopLabel = false;
        mPlayTask = null;
        updateControls();
    }

    /**
     * Sets the TextureView transform to preserve the aspect ratio of the video.
     */
    private void adjustAspectRatio(int videoWidth, int videoHeight) {
        int viewWidth = mTextureView.getWidth();
        int viewHeight = mTextureView.getHeight();
        double aspectRatio = (double) videoHeight / videoWidth;

        int newWidth, newHeight;
        if (viewHeight > (int) (viewWidth * aspectRatio)) {
            // limited by narrow width; restrict height
            newWidth = viewWidth;
            newHeight = (int) (viewWidth * aspectRatio);
        } else {
            // limited by short height; restrict width
            newWidth = (int) (viewHeight / aspectRatio);
            newHeight = viewHeight;
        }
        int xoff = (viewWidth - newWidth) / 2;
        int yoff = (viewHeight - newHeight) / 2;
        Log.v(TAG, "video=" + videoWidth + "x" + videoHeight +
                " view=" + viewWidth + "x" + viewHeight +
                " newView=" + newWidth + "x" + newHeight +
                " off=" + xoff + "," + yoff);

        Matrix txform = new Matrix();
        mTextureView.getTransform(txform);
        txform.setScale((float) newWidth / viewWidth, (float) newHeight / viewHeight);
        //txform.postRotate(10);          // just for fun
        txform.postTranslate(xoff, yoff);
        mTextureView.setTransform(txform);
    }



    /**
     * Updates the on-screen controls to reflect the current state of the app.
     */
    private void updateControls() {
        Button play = (Button) findViewById(R.id.play_stop_button);
        if (mShowStopLabel) {
            play.setText(R.string.stop_button_text);
        } else {
            play.setText(R.string.play_button_text);
        }
//        play.setEnabled(mSurfaceTextureReady);

        // We don't support changes mid-play, so dim these.
        CheckBox check = (CheckBox) findViewById(R.id.locked60fps_checkbox);
        check.setEnabled(!mShowStopLabel);
        check = (CheckBox) findViewById(R.id.loopPlayback_checkbox);
        check.setEnabled(!mShowStopLabel);
    }

}
