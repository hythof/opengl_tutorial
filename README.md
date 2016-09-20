# OpenGL最小入門
OpenGLに少し勉強してみると意外と簡単だったのでご紹介します。
3Dって難しそうというイメージがあるかもしれませんが、大まかなイメージはすぐに掴めると思います。
この記事では以下を説明します。

- 頂点と描画例
- 透視投影とは
- シェーダーとは

また以下の事は扱いません。
- UVと画像表示
- ファイル読み込み
- 様々な最適化

## 頂点と描画例
頂点とは3D空間の中にある1つの座標をx, y, zの3つの小数で表します。
これから点、線、三角形、四角形といった簡単な図形の描画例を見ていきます。

### まずは頂点を1つ表示
座標情報と点の大きさを指定します。大きさを指定しないと点が小さすぎて見つけにくいので、大きめを指定します。
```cpp
model.SetVertices({
	 0., 0, 0,
});
model.DrawLines(100.0f);
```
描画するとこんな感じ。座標0が真ん中を表します。
![dot.png](https://qiita-image-store.s3.amazonaws.com/0/39587/4638725c-800d-24f4-aa8b-ab38b78adb30.png)

### 次に頂点を2つ表示
もう一つ座標を追加し2つの頂点を指定します。
```cpp
model.SetVertices({
	 0.5f, 0, 0,
	-0.5f, 0, 0,
});
model.SetDrawLines(100.0f);
```
右上に点が追加されました。x座標が正だと右、y座標が正だと上に移動します。負の場合はそれぞれ逆方向に移動します。
![two_dot.png](https://qiita-image-store.s3.amazonaws.com/0/39587/dc9c4919-0c30-c764-0a2f-fbb87904f698.png)

### 線を表示
座標を2つと「線を引く」命令で点が2ではなく、線となります。
```cpp
model.SetVertices({
     0.5f, 0, 0,
    -0.5f, 0, 0,
});
model.DrawLines(100.0f);
```
![line.png](https://qiita-image-store.s3.amazonaws.com/0/39587/dd454ae5-f336-a2e4-debf-f3826731e128.png)

次に線を2本引いてみます。
```cpp
model.SetVertices({
     0.5f, 0, 0,
     -0.5f, 0, 0,
     0.5f, 0.5f, 0,
     -0.5f, 0.5f, 0,
});
model.DrawLines(100.0f);
```

簡単ですね！
![two_line.png](https://qiita-image-store.s3.amazonaws.com/0/39587/7dcac080-cbbe-aea4-6fb5-55b4cb1c38e2.png)

### そして三角形を表示
三角形はこれまでと少し違います。頂点3つに加え、どの順番で頂点を描画するか順序を指定しないといけません。この順序は頂点インデックスと呼ばれています。

```cpp
model.SetVertices({
     0.5f, -0.5f, 0,![index.png]
        0,  0.5f, 0,
    -0.5f, -0.5f, 0,
});
model.DrawTriangles({
    0, 1, 2
});
```
![triangle.png](https://qiita-image-store.s3.amazonaws.com/0/39587/0c9af7d0-3060-25f2-1513-600605c6ad5f.png)

描画順はこんな感じです。
![index.png](https://qiita-image-store.s3.amazonaws.com/0/39587/0f93cdcf-3103-5245-2952-87c1e73c7d41.png)

### 最後に四角形を表示
四角形は2つの三角形を合わせて描画します。ここでは4つの頂点と頂点インデックスの組み合わせで、2つの三角形を表現しています。
```cpp
model.SetVertices({
     0.5, -0.5, 0,
     0.5,  0.5, 0,
    -0.5,  0.5, 0,
    -0.5, -0.5, 0,
});
model.DrawTriangles({
    0, 1, 2,
    0, 2, 3,
});
```
![quadrangle.PNG](https://qiita-image-store.s3.amazonaws.com/0/39587/8205cf24-efea-3d67-5eae-c3cf2416b713.png)

## 透視投影とは
透視投影(perspective projection)という図法を使えば3Dっぽく見せることができます。近くのものはより大きく、遠くのものはより小さく写す事で遠近感を出すものです。

透視投影のイメージ図。zが0に近い程に大きく、zが大きくなるに連れ小さく描画されます。
>![透視投影法のイメージ画像](http://marupeke296.com/images/DXG_No70_img0002.gif "")
>[ゲームつくろー！の「② 視錐台を描画空間に変換する」より]([http://marupeke296.com/DXG_No70_perspective.html)
> 引用元サイトで透視投影の理論と計算方法の紹介もあります

これまでと比べて少し複雑ですが、透視投影を使い右斜上からの視点で正方形を描画してみます。
```cpp
// 正方形の頂点は8点、各頂点はx y zの3つを持つ
model.SetVertices({
     1.0, -1.0, -1.0,
     1.0, -1.0,  1.0,
    -1.0, -1.0,  1.0,
    -1.0, -1.0, -1.0,

     1.0,  1.0, -1.0,
     1.0,  1.0,  1.0,
    -1.0,  1.0,  1.0,
    -1.0,  1.0, -1.0
});
// 正方形の面は6つ、それぞれの面を2つの三角形で表現
model.DrawTriangles({
    1, 3, 0,
    7, 5, 4,

    4, 1, 0,
    5, 2, 1,

    2, 7, 3,
    0, 7, 4,

    1, 2, 3,
    7, 6, 5,

    4, 5, 1,
    5, 6, 2,

    2, 6, 7,
    0, 3, 7
});

// カメラの座標(4, 2, 6)から、正方形の座標(0, 0, 0)を見る射影行列を求める
glm::mat4 mvp = camera.LookAt(glm::vec3(4, 2, 6), glm::vec3(0, 0, 0));
model.SetMVP(mvp);
```
![cube.PNG](https://qiita-image-store.s3.amazonaws.com/0/39587/7d3af425-10f3-9dd6-d58b-5d0d67dea82d.png)

## シェーダーとは
大きく２つの種類があります。「頂点シェーダー」と「フラグメントシェーダー」です。フラグメントシェーダはOpenGLの用語でDirectXでは「ピクセルシェーダー」と呼ばれています。

頂点シェーダーの例。
全ての頂点に同じ処理を行えます。
positionにプログラムで指定した(-1.0, 0.0, -0.5)とかの座標が、mvpには透視投影を行うための行列情報が格納されます。
```glsl
#version 450
layout (location = 0) in vec3 position;
uniform mat4 mvp;
void main()
{
    gl_Position = mvp * vec4(position, 1.0);
}
```

フラグメントシェーダーの例。
描画対象のピクセルの色を決めています。
vec4(赤, 緑, 青, 透明度)を表し、赤緑青の場合は全て1.0で白、全て0.0で黒を表しその中間で様々な色を表現します。透明度は1.0で不透明、0.0で透明(見えない)、その中間で半透明を表しています。
```glsl
#version 450
out vec4 out_color;
void main()
{
    out_color = vec4(1.0, 1.0, 1.0, 1.0);
}
```

## 終わりに
記事で用いたサンプルプログラムと、更に進んだ内容を勉強するための参考URLを記載しておきます。

## 参考URL
- [OpenGL チュートリアル](http://www.opengl-tutorial.org/jp/)
- [今どきのOpenGLとC言語で初音ミクを表示する(その1): ウインドウの表示](http://qiita.com/lambdataro/items/02668f5e542ffe9e80df)
- [ゲームつくろー](http://marupeke296.com/GameMain.html)

## サンプルプログラムの構成
[githubのサンプルコード](https://github.com/hythof/opengl_tutorial)
```shell-session
git clone https://github.com/hythof/opengl_tutorial.git
```

動作確認環境
- Windows 7 (64 bit)
- Microsoft Visual Studio 2015 Pro
- [GLFW](http://www.glfw.org/) 3.2. ([zlib/libpng license](http://www.glfw.org/license.html))
- [GLEW](http://glew.sourceforge.net/) 2.0 ([license link](https://github.com/nigels-com/glew#copyright-and-licensing))
- [glm](http://glm.g-truc.net/0.9.8/index.html) 0.9.8.0 ([MIT license](http://glm.g-truc.net/copying.txt))


| パス                        | 説明                              |
|-----------------------------|-----------------------------------|
| src/gldemo/gldemo.sln       | VS2015のソリューション            |
| src/gldemo/gldemo/main.cpp  | サンプルプログラムのmain関数      |
| src/gldemo/vendors/         | 一般公開されているライブラリ      |