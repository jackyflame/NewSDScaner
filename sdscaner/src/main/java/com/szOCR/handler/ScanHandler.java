
package com.szOCR.handler;

import android.app.Activity;
import android.content.Intent;
import android.graphics.Bitmap;
import android.net.Uri;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;

import com.szOCR.R;
import com.szOCR.activity.ScanView;
import com.szOCR.camera.CameraPreview;
import com.szOCR.general.CGlobal;
import com.szOCR.general.RecogResult;

/* This class handles all the messaging which comprises the state machine for capture. */
public final class ScanHandler extends Handler {

	private static final String TAG = ScanHandler.class.getSimpleName();

	private final ScanView mActivity;
	private final CameraPreview mCameraPreview;
	private final RecogThread decodeThread;
	private State state;
	 
	private enum State {PREVIEW, SUCCESS, DONE}

	public ScanHandler(ScanView activity,CameraPreview cameraPreview){
		this.mActivity = activity;
		this.mCameraPreview = cameraPreview;
		decodeThread = new RecogThread(activity);
		decodeThread.start();
		state = State.SUCCESS;

		restartPreviewAndDecode();
	}

	@Override
	public void handleMessage(Message message) {
        if (message.what == R.id.auto_focus) {// Log.d(TAG, "Got auto-focus message");
            // When one auto focus pass finishes, start another. This is the closest thing to continuous AF.
            // It does seem to hunt a bit, but I'm not sure what else to do.
            //if (state == State.PREVIEW) {
            //	mCameraPreview.requestAutoFocus(this, R.id.auto_focus);
            //}
            mCameraPreview.autoCameraFocuse();
        } else if (message.what == R.id.restart_preview) {
            Log.d(TAG, "Got restart preview message");
            restartPreviewAndDecode();
        } else if (message.what == R.id.recog_succeeded) {
            Log.d(TAG, "Got decode succeeded message");
            state = State.SUCCESS;
            Bundle bundle = message.getData();
            Bitmap bmImage = bundle == null ? null : (Bitmap) bundle.getParcelable(CGlobal.PHONENUMBER_BITMAP);//
            mActivity.returnRecogedData((RecogResult) message.obj, bmImage);//
        } else if (message.what == R.id.recog_failed) {// We're decoding as fast as possible, so when one decode fails, start another.
            state = State.PREVIEW;
            mCameraPreview.requestPreviewFrame(decodeThread.getHandler(), R.id.recog_start);
        } else if (message.what == R.id.return_scan_result) {
            Log.d(TAG, "Got return scan result message");
            mActivity.getActivityContext().setResult(Activity.RESULT_OK, (Intent) message.obj);
            mActivity.getActivityContext().finish();
        } else if (message.what == R.id.launch_product_query) {
            Log.d(TAG, "Got product query message");
            String url = (String) message.obj;
            Intent intent = new Intent(Intent.ACTION_VIEW, Uri.parse(url));
            intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_WHEN_TASK_RESET);
            mActivity.getActivityContext().startActivity(intent);
        }
	}

	public void quitSynchronously() {
		state = State.DONE;
		//mCameraPreview.stopPreview();
		Message quit = Message.obtain(decodeThread.getHandler(), R.id.quit);
		quit.sendToTarget();
		try 
		{
			decodeThread.join();
		} 
		catch (InterruptedException e) {
			// continue
		}
		// Be absolutely sure we don't send any queued up messages
		removeMessages(R.id.recog_succeeded);
		removeMessages(R.id.recog_failed);
		removeMessages(R.id.auto_focus);
	}

	private void restartPreviewAndDecode() {
		if (state == State.SUCCESS) {
			state = State.PREVIEW;
			mCameraPreview.requestPreviewFrame(decodeThread.getHandler(),R.id.recog_start);
			//mCameraPreview.requestAutoFocus(this, R.id.auto_focus);
			//this.sendEmptyMessageDelayed(R.id.auto_focus, 1500);
		}
	}

}
