package com.jack.splayer;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.view.SurfaceHolder;
import android.view.View;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

/**
 * @创建者 Jack
 * @创建时间 2025-07-07 22:22
 * @描述 Android 8.0 声音重音（最大的可能是声音打开了两次） 视频不能显示的问题   适配处理
 * 继承 GLSurfaceView.Renderer 接口
 */
public class SPlay extends GLSurfaceView implements SurfaceHolder.Callback, GLSurfaceView.Renderer, View.OnClickListener {

    public SPlay(Context context, AttributeSet attrs) {
        super(context, attrs);

        setOnClickListener( this );

        //android 8.0 需要设置
        setRenderer(this);//该方法只需要调用一次，放在构造方法中
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        //        super.surfaceCreated(holder);
        InitView(holder.getSurface());

        //只有在绘制数据改变时才绘制view，可以防止GLSurfaceView帧重绘
        //        setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int w, int h) {
        //        super.surfaceChanged(holder, format, w, h);
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        //        super.surfaceDestroyed(holder);
    }

    public native void InitView(Object surface);

    @Override
    public void onDrawFrame(GL10 gl10) {

    }

    @Override
    public void onSurfaceChanged(GL10 gl10, int i, int i1) {

    }

    @Override
    public void onSurfaceCreated(GL10 gl10, EGLConfig eglConfig) {

    }

    @Override
    public void onClick(View view) {
        PlayOrPause();
    }

    public native void PlayOrPause();
}
