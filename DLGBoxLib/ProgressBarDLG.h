#pragma once

/**
@brief     再帰定義可能なプログレスバーカウンタ
@file      ProgressBarDLG.h
@author    Fumi.Iseki (C)
*/

#include  "DLGBoxBase.h"

/**
@file      ProgressBarDLG.h

@par 再帰定義可能なビジュアルカウンタ for VC++
これは，VC++で使用できるプログレスバーカウンタである．動的に目盛幅を変更できるため，カウンタ
作成時に作業量の推定が困難な場合でも，ある程度の予測で使用することができる．@n
また，再帰的に仮想的なチャイルドカウンタをつくることができるので，呼び出した関数にその操作を任せる
ことができる．@n
呼び出される関数のなかでは CVCounter（このカウンタのスパークラス）のカウンタ作っておけば十分で，
呼び出し側で，このカウンタの使用・不使用を決めることができ，汎用性が高い（バーチャル関数の機能による）．
ディスパッチャーを有効に設定（コンストラクタで設定，デフォルトは無効）すると，PutFill(), SetPos(), 
StepIt()関数の最後でディスパッチャー関数 DisPatcher()が起動される（classBox.h参照）．@n

ディスパッチャーが有効な時，システムメニューのクローズボタンをクリックすると，isCanceled()関数がtrue
になる． 中断可能な作業では，定期的にこの関数をチェックし，trueなら作業を中断することは理にかなった
処理である．@n

全体の目盛に対して，刻み幅が非常に小さいと，関数の呼び出しが重なり，それだけで重い処理になってしまう．
従って，関連関数（主に上記３種）の呼び出し回数はなるべく少なくなるようにすべきである．@n

@attention
リソース IDD_PROGBAR が必要

@see classBox.h

@par 使い方 （詳しくはソースを見よ）

    １．まず，関数内で使用するためのカウンタへのポインタを宣言する．
        CProgressBarDLG* counter = NULL;

    ２．カウンタインスタンスの取得
        1) カウンタを生成する．
                counter = new CProgressBarDLG(IDD_PROGBAR);
                        or
        2) または，どこか別の所で定義されたカウンタの目盛りの内，この関数内で自由に使用して
           よい目盛りの範囲（領域，部分）を得る    （チャイルドカウンタを得る）
                counter = (CProgressBarDLG*)(CNT->GetUsableCounter());  
                    // CNTは何処か（例えば呼び出し側関数）で宣言されたカウンタ
                        or
        3) または，グローバルカウンタ（のチャイルドカウンタ）を使用する
                counter = GetUsableGlobalCounter();
           ただし，グローバルカウンタはどこかで設定されていなければならない．
                設定法
                CProgressBarDLG* counter = new CProgressBarDLG(IDD_PROGBAR);
                SetGlobalCounter(counter);

    ３．カウンタをスタートさせてプログレスバーを表示する．
                counter->Start();  or Start(m)    m は SetMax(m) される．
        タイトルの設定は，スタート後でもＯＫ．
                counter->SetTitle("タイトル");

    ４．今使えるカウンタ領域に，1〜200までの目盛りを設定
                counter->SetMax(200);           

    ５．200のうちの 1〜80までを 100でスケーリングし直す．ローカル目標は80(100)に設定．
                counter->ResetRate(80, 100);    

    ６．カウンタを1つ(1/100)上げる．実際に表示される目盛りは，ローカル目標を超えることはない．
                counter->StepIt();

    ７．ローカル目標まで，目盛りの領域を埋める．ResetRate() の効果はここで終了
                counter->PutFill();             

    ８．チャイルドカウンタを作る場合（例）．
        1) 残り81〜100で，チャイルドカウンターを作る．
                counter->MakeChildCounter(20);   
        2) 関数を呼び出し，チャイルドカウンタの取り扱いは呼び出した関数に任せる
           呼び出した関数の中で，再びチャイルドカウンタが有効領域として使用される
        3) 呼び出した関数が終了したら，チャイルドカウンタを取り消し，親関数へカウンタを返す
                counter->DeleteChildCounter();  

    ９. カウンタ使用の終了
        1) 自分で作った場合は delete する．
        2) 何処かで作られたカウンタの場合はそまま何もしない．
        3) グローバルカウンタを本体を自分で作成した場合は，グローバルカウンタをクリアして deleteする．

@par 例
@code
    CProgressBarDLG* counter = new CProgressBarDLG(IDD_PROGBAR);
    SetGlobalCounter(counter);
    .....................
    ClearGlobalCounter();   または   SetGlobalCounter(NULL);
    delete(counter);
@endcode
@cpde
    CNT xxxxxxxxxxxx--------------------------xxxxxxxxxxx   この親関数は xxxx の部分を使用できる
                        ↓関数呼び出し
                   ↑-------------------ccccc↑  counter  （---- は自分で使用できる範囲）
                                                   ↓呼び出し （cccc の部分は呼び出した関数にお任せ）
                                       ↑cccc↑  チャイルドカウンタ
@endcode

*/


#define  PROGRESS_COUNTERMAX  3200    // カウンタの目盛の最大値（short int）



//
namespace jbxwl {

using namespace jbxl;



/////////////////////////////////////////////////////////////////////////////
// CProgressBarDLG ダイアログ

class CProgressBarDLG : public CDialog, public CVCounter
{
public:
    CWnd*       pWnd;
    UINT        dialogID;
    CString     title;
    BOOL        enableDisPatcher;   

    int         gmax;
    int         base;
    double      grate;
    double      lrate;
    double      prate;

    CProgressCtrl*    progBar;
    CProgressBarDLG*  parent;
    CProgressBarDLG*  child;

// コンストラクション
public:
    CProgressBarDLG(                  LPCTSTR tname=NULL, BOOL dispatcher=FALSE, CWnd* pParent=NULL);
    CProgressBarDLG(UINT nIDTemplate, LPCTSTR tname=NULL, BOOL dispatcher=FALSE, CWnd* pParent=NULL);
    CProgressBarDLG(CProgressBarDLG* counter, LPCTSTR tname=NULL, BOOL dispatcher=FALSE);
    virtual ~CProgressBarDLG();

    BOOL  OnInitDialog();
    
    virtual CVCounter* MakeChildCounter(int m);
    virtual CVCounter* GetUsableCounter();
    virtual void  DeleteChildCounter();

    virtual void  Start(int m=100, char* tn=NULL);  // カウンタスタート
    virtual void  Stop();                           // 一時停止

    virtual void  Init();
    virtual void  Clear();
    virtual void  SetMax(int max);
    virtual void  SetPos(int pos);
    virtual void  StepIt(int n=1);
    virtual void  ResetRate(int m, int n);
    virtual void  PutFill();
    virtual void  SetTitle(char* tn);

    virtual bool  isCanceled();

// ダイアログ データ
    //{{AFX_DATA(CProgressBarDLG)
    enum { IDD = IDD_PROGBAR };
    //}}AFX_DATA

// オーバーライド
protected:
    //{{AFX_VIRTUAL(CProgressBarDLG)
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);   // ウィンドウクローズ用 
    //}}AFX_VIRTUAL

// インプリメンテーション
protected:
    //{{AFX_MSG(CProgressBarDLG)
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};


//{{AFX_INSERT_LOCATION}}


}       // namespace

