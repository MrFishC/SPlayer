package com.jack.splayer

import android.content.pm.ActivityInfo
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.view.Window
import android.view.WindowManager
import android.widget.TextView
import com.jack.splayer.databinding.ActivityMainBinding

class MainActivity : AppCompatActivity() {

//    private lateinit var binding: ActivityMainBinding

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        //去掉标题栏
        supportRequestWindowFeature(Window.FEATURE_NO_TITLE);
        //全屏，隐藏状态
        window.setFlags(
            WindowManager.LayoutParams.FLAG_FULLSCREEN,
            WindowManager.LayoutParams.FLAG_FULLSCREEN
        )

        //加了这块代码显示会存在问题 但是在配置文件中设置 android:screenOrientation="landscape" 就可以正常显示 todo
        //屏幕为横屏
//        requestedOrientation = ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE

//        binding = ActivityMainBinding.inflate(layoutInflater)
//        setContentView(binding.root)
        setContentView(R.layout.activity_main)

        // Example of a call to a native method
//        binding.sampleText.text = stringFromJNI()
        stringFromJNI()
    }

    /**
     * A native method that is implemented by the 'splayer' native library,
     * which is packaged with this application.
     */
    external fun stringFromJNI(): String

    companion object {
        // Used to load the 'splayer' library on application startup.
        init {
            System.loadLibrary("splayer")
        }
    }
}