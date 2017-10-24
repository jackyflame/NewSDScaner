package com.szOCR.handler;

import java.util.concurrent.CountDownLatch;

import android.os.Handler;
import android.os.Looper;

import com.szOCR.activity.ScanActivity;

/**
 * This thread does all the heavy lifting of decoding the images. 
 */
final class RecogThread extends Thread {

	public static final String RECOG_BITMAP = "recog_bitmap";
	private final ScanActivity activity;
	private Handler handler;
	private final CountDownLatch handlerInitLatch;

	RecogThread(ScanActivity activity) {
		this.activity = activity;
		handlerInitLatch = new CountDownLatch(1);
	}

	Handler getHandler() {
		try {
			handlerInitLatch.await();
		} catch (InterruptedException ie) {
			// continue?
		}
		return handler;
	}

	@Override
	public void run() {
		Looper.prepare();
		handler = new RecogHandler(activity);
		handlerInitLatch.countDown();
		Looper.loop();
	}

}
