/*******************************************************************************
**                                                                            **
**                     Jiedi(China nanjing)Ltd.                               **
**	               创建：夏曹俊，此代码可用作为学习参考                       **
*******************************************************************************/

/*****************************FILE INFOMATION***********************************
**
** Project       : FFmpeg
** Description   : FFMPEG项目创建示例
** Contact       : xiacaojun@qq.com
**        博客   : http://blog.csdn.net/jiedichina
**		视频课程 : 网易云课堂	http://study.163.com/u/xiacaojun		
				   腾讯课堂		https://jiedi.ke.qq.com/				
				   csdn学院		http://edu.csdn.net/lecturer/lecturer_detail?lecturer_id=961	
**                 51cto学院	http://edu.51cto.com/lecturer/index/user_id-12016059.html	
** 				   下载最新的ffmpeg版本 ffmpeg.club
**                 
**   安卓流媒体播放器 课程群 ：23304930 加入群下载代码和交流
**   微信公众号  : jiedi2007
**		头条号	 : 夏曹俊
**
*******************************************************************************/
//！！！！！！！！！ 加群23304930下载代码和交流


package com.jack.splayer;

import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;

import androidx.appcompat.app.AppCompatActivity;

/**
 * Created by Administrator on 2018-03-11.
 */

public class OpenUrl extends AppCompatActivity {
    private Button btfile;
    private Button btrtmp;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate( savedInstanceState );
        setContentView( R.layout.openurl );
        btfile = findViewById( R.id.playvideo );
        btfile.setOnClickListener(
                new View.OnClickListener() {
                   @Override
                   public void onClick(View view) {
                       EditText t = findViewById( R.id.fileurl );
                       //用户输入的URL，打开视频
                       Open(t.getText().toString());

                       //关闭当前窗口
                       finish();
                   }
               }
        );


        btrtmp = findViewById( R.id.playrtmp );
        btrtmp.setOnClickListener(
                new View.OnClickListener() {
                    @Override
                    public void onClick(View view) {
                        EditText t = findViewById( R.id.rtmpurl );
                        //用户输入的URL，打开视频
                        Open(t.getText().toString());

                        //关闭当前窗口
                        finish();
                    }
                }
        );


    }
    public native void Open(String url);
}
