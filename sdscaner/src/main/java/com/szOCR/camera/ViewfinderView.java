

package com.szOCR.camera;

import android.content.Context;
import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;
import android.util.AttributeSet;
import android.view.View;

import com.szOCR.R;
import com.szOCR.general.CGlobal;

/**
 * This view is overlaid on top of the camera preview. It adds the viewfinder rectangle and partial
 * transparency outside it, as well as the laser scanner animation and result points.
 * 
 */
public final class ViewfinderView extends View 
{

  private static final int[] SCANNER_ALPHA = {0, 64, 128, 192, 255, 192, 128, 64};
  private static final long ANIMATION_DELAY = 500L;//100L;
  private static final int OPAQUE = 0xFF;

  private final Paint paint;
  private Bitmap resultBitmap;
  private final int frameColor;
  private final int laserColor;
  private int scannerAlpha;
  
  public int m_top = 0;
  public int m_height = 0;
  private float delta;
  
  private long stTime;
  // This constructor is used when the class is built from an XML resource.
  public ViewfinderView(Context context, AttributeSet attrs) 
  {
	  	super(context, attrs);

	    // Initialize these once for performance rather than calling them every time in onDraw().
	    paint = new Paint();
	    Resources resources = getResources();
	    frameColor = resources.getColor(R.color.viewfinder_frame);
	    laserColor = resources.getColor(R.color.viewfinder_laser);
	    scannerAlpha = 0;
	    
	    delta = 1.0F;
	    delta = getResources().getDisplayMetrics().density / 1.5F;
	    
	    stTime = System.currentTimeMillis();
 }

  @Override
  public void onDraw(Canvas canvas) 
  {
	    //Rect frame = new Rect(50,50,320,320);//CameraManager.get().getFramingRect());
	  	Rect frame = CGlobal.g_rectPreview;
	  	
	    if (frame == null) 
	    {
	      return;
	    }
	    
	    int width = canvas.getWidth();
	    int height = canvas.getHeight();
	
	    // Draw the exterior (i.e. outside the framing rect) darkened
	    //paint.setColor(resultBitmap != null ? resultColor : maskColor);
	    paint.setColor(0x55555555);//77777777);
	    //paint.setAlpha(OPAQUE);
	    paint.setAlpha(200);
	    
	    canvas.drawRect(0, 0, width, frame.top, paint);
	    canvas.drawRect(0, frame.top, frame.left, frame.bottom + 1, paint);
	    canvas.drawRect(frame.right + 1, frame.top, width, frame.bottom + 1, paint);
	    canvas.drawRect(0, frame.bottom + 1, width, height, paint);
	
	    if (resultBitmap != null) {
	      // Draw the opaque result bitmap over the scanning rectangle
	    	paint.setAlpha(OPAQUE);
	    	canvas.drawBitmap(resultBitmap, frame.left, frame.top, paint);
	    } else {
	
	      // Draw a two pixel solid black border inside the framing rect
	      paint.setColor(frameColor);
	      canvas.drawRect(frame.left, frame.top, frame.right + 1, frame.top + 2, paint);
	      canvas.drawRect(frame.left, frame.top + 2, frame.left + 2, frame.bottom - 1, paint);
	      canvas.drawRect(frame.right - 1, frame.top, frame.right + 1, frame.bottom - 1, paint);
	      canvas.drawRect(frame.left, frame.bottom - 1, frame.right + 1, frame.bottom + 1, paint);
	      
	      int conner;
	      conner = (frame.bottom - frame.top) / 8;
	        
	      //paint.setStyle(android.graphics.Paint.Style.FILL_AND_STROKE);//FILL);
	      if(CGlobal.g_bAutoFocused == false)	paint.setColor(Color.RED);
	      else									paint.setColor(Color.GREEN);
	      //paint.setColor(-0xFF0100);
	      paint.setColor(Color.GREEN);
	      canvas.drawRect(createRect(frame.left, frame.top, frame.left + conner, frame.top), paint);
	      canvas.drawRect(createRect(frame.left, frame.top, frame.left, frame.top + conner), paint);
	      canvas.drawRect(createRect(frame.right, frame.top, frame.right - conner, frame.top), paint);
	      canvas.drawRect(createRect(frame.right, frame.top, frame.right, frame.top + conner), paint);
	      canvas.drawRect(createRect(frame.left, frame.bottom, frame.left + conner, frame.bottom), paint);
	      canvas.drawRect(createRect(frame.left, frame.bottom, frame.left, frame.bottom - conner), paint);
	      canvas.drawRect(createRect(frame.right, frame.bottom, frame.right - conner, frame.bottom), paint);
	      canvas.drawRect(createRect(frame.right, frame.bottom, frame.right, frame.bottom - conner), paint);
	        
	
	      // Draw a red "laser scanner" line through the middle to show decoding is active
	      paint.setColor(laserColor);
	      paint.setAlpha(SCANNER_ALPHA[scannerAlpha]);
	      scannerAlpha = (scannerAlpha + 1) % SCANNER_ALPHA.length;
	      int middle = frame.height() / 2 + frame.top;
	      canvas.drawRect(frame.left + 2, middle - 1, frame.right - 1, middle + 2, paint);
	
	      
	      // Request another update at the animation interval, but only repaint the laser line,
	      // not the entire viewfinder mask.
	      postInvalidateDelayed(ANIMATION_DELAY, frame.left, frame.top, frame.right, frame.bottom);
	   }
  }
  private Rect createRect(int left, int top, int right, int bottom)
  {
      int offset = (int)(2F * delta);
      Rect rect = new Rect();
      rect.left = Math.min(left, right) - offset;
      rect.right = Math.max(left, right) + offset;
      rect.top = Math.min(top, bottom) - offset;
      rect.bottom = Math.max(top, bottom) + offset;
      return rect;
  }
  
  public void drawViewfinder() 
  {
    resultBitmap = null;
    invalidate();
  }

  
}
