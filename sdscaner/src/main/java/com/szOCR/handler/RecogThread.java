package com.szOCR.handler;

import android.os.Handler;
import android.os.Looper;

import com.szOCR.activity.ScanView;

import java.util.concurrent.CountDownLatch;

/**
 * This thread does all the heavy lifting of decoding the images. 
 */
final class RecogThread extends Thread {

	public static final String RECOG_BITMAP = "recog_bitmap";
	private final ScanView activity;
	private Handler handler;
	private final CountDownLatch handlerInitLatch;

	RecogThread(ScanView activity) {
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
