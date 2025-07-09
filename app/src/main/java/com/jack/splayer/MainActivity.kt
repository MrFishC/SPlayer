package com.jack.splayer

import android.content.Intent
import android.os.Bundle
import android.util.Log
import android.view.Window
import android.view.WindowManager
import android.widget.SeekBar
import androidx.appcompat.app.AppCompatActivity
import com.jack.splayer.databinding.ActivityMainBinding

class MainActivity : AppCompatActivity(), Runnable, SeekBar.OnSeekBarChangeListener {

    private lateinit var binding: ActivityMainBinding

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

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        binding.aplayseek.max = 1000
        binding.aplayseek.setOnSeekBarChangeListener(this)

        binding.openButton.setOnClickListener {
            Log.e("SPlay", "open button click!")
            //打开选择路径窗口
            val intent = Intent()
            intent.setClass(this@MainActivity, OpenUrl::class.java)
            startActivity(intent)
        }

        //启动播放进度线程
        Thread(this).start()
    }

    external fun Seek(pos: Double)
    external fun PlayPos(): Double

    override fun run() {
        while (true) {
            binding.aplayseek.progress = (PlayPos() * 1000).toInt()
            try {
                Thread.sleep(40)
            } catch (e: InterruptedException) {
                e.printStackTrace()
            }
        }
    }

    override fun onProgressChanged(
        p0: SeekBar?,
        p1: Int,
        p2: Boolean
    ) {
    }

    override fun onStartTrackingTouch(p0: SeekBar?) {
    }

    override fun onStopTrackingTouch(p0: SeekBar?) {
        Seek(binding.aplayseek.progress.toDouble() / binding.aplayseek.max.toDouble())
    }

    companion object {
        // Used to load the 'splayer' library on application startup.
        init {
            System.loadLibrary("splayer")
        }
    }
}