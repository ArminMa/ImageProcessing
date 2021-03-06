// Sharpen.cpp : 实现文件
//

#include "stdafx.h"
#include "OpenCVPlat.h"
#include "Sharpen.h"
#include "afxdialogex.h"
#include "MainFrm.h"
#include "OpenCVPlatDoc.h"
#include "OpenCVPlatView.h"

// CSharpen 对话框

IMPLEMENT_DYNAMIC(CSharpen, CDialogEx)

CSharpen::CSharpen(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSharpen::IDD, pParent)
{
	CMainFrame* pMain = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	COpenCVPlatDoc *pDoc = (COpenCVPlatDoc*)pMain->GetActiveDocument();
	old_img = pDoc->image.clone();
	sharpened_img = old_img.clone();
}

CSharpen::~CSharpen()
{
}

void CSharpen::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSharpen, CDialogEx)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_ROBERTS, &CSharpen::OnBnClickedRoberts)
	ON_BN_CLICKED(IDC_PREWITT, &CSharpen::OnBnClickedPrewitt)
	ON_BN_CLICKED(IDC_SOBEL, &CSharpen::OnBnClickedSobel)
	ON_BN_CLICKED(IDC_LAPLACE_3X3, &CSharpen::OnBnClickedLaplace3x3)
	ON_BN_CLICKED(IDC_COMPOSITE_LAPLACIAN_MASK5, &CSharpen::OnBnClickedCompositeLaplacianMask5)
	ON_BN_CLICKED(IDC_COMPOSITE_LAPLACIAN_MASK9, &CSharpen::OnBnClickedCompositeLaplacianMask9)
END_MESSAGE_MAP()


// CSharpen 消息处理程序


void CSharpen::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
	scroll();
}

void CSharpen::scroll()
{
	CSliderCtrl *pSlidCtrl = (CSliderCtrl*)GetDlgItem(IDC_SHARPEN);
	double value = pSlidCtrl->GetPos() / 100.0;
	
	CMainFrame* pMain = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	COpenCVPlatDoc *pDoc = (COpenCVPlatDoc*)pMain->GetActiveDocument();
	COpenCVPlatView *pView = (COpenCVPlatView*)pMain->GetActiveView();
	int height = pDoc->image.size().height;
	int width = pDoc->image.size().width;
	for (int row = 0; row < height - 1; row++)
	{
		uchar *old_data = old_img.ptr<uchar>(row);
		uchar *data = pDoc->image.ptr<uchar>(row);
		uchar *sharp_data = sharpened_img.ptr<uchar>(row);
		for (int col = 0; col < width - 1; col++)
		{
			if (pDoc->image.channels() == 3)
			{
				data[col * 3] = (1 - value) * old_data[col * 3] + value * sharp_data[col * 3];
				data[col * 3 + 1] = (1 - value) * old_data[col * 3 + 1] + value * sharp_data[col * 3 + 1];
				data[col * 3 + 2] = (1 - value) * old_data[col * 3 + 2] + value * sharp_data[col * 3 + 2];
			}
			else
			{
				data[col] = (1 - value) * old_data[col] + value * sharp_data[col];
			}
		}
	}
	pView->Invalidate();
}


BOOL CSharpen::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CSliderCtrl *pSlidCtrl = (CSliderCtrl*)GetDlgItem(IDC_SHARPEN);
	pSlidCtrl->SetRange(0, 100);
	pSlidCtrl->SetPos(0);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}


void CSharpen::OnBnClickedRoberts()
{
	// TODO:  在此添加控件通知处理程序代码
	CMainFrame* pMain = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	COpenCVPlatDoc *pDoc = (COpenCVPlatDoc*)pMain->GetActiveDocument();
	COpenCVPlatView *pView = (COpenCVPlatView*)pMain->GetActiveView();
	int height = pDoc->image.size().height;
	int width = pDoc->image.size().width;
	int *praw = new int[width * height * pDoc->image.channels()];
	double max_r = 0;
	double max_g = 0;
	double max_b = 0;
	for (int row = 0; row < height - 1; row++)
	{
		uchar *old_data = old_img.ptr<uchar>(row);
		int *raw = praw + row * width * pDoc->image.channels();
		for (int col = 0; col < width - 1; col++)
		{
			if (pDoc->image.channels() == 3)
			{
				int z5_b = old_data[col * 3];
				int z5_g = old_data[col * 3 + 1];
				int z5_r = old_data[col * 3 + 2];

				int z6_b = old_data[col * 3 + 3];
				int z6_g = old_data[col * 3 + 4];
				int z6_r = old_data[col * 3 + 5];

				int z8_b = old_data[col * 3 + width * 3];
				int z8_g = old_data[col * 3 + width * 3 + 1];
				int z8_r = old_data[col * 3 + width * 3 + 2];

				int z9_b = old_data[col * 3 + width * 3 + 3];
				int z9_g = old_data[col * 3 + width * 3 + 4];
				int z9_r = old_data[col * 3 + width * 3 + 5];

				raw[col * 3] = abs(z5_b - z9_b) + abs(z6_b - z8_b);
				raw[col * 3 + 1] = abs(z5_g - z9_g) + abs(z6_g - z8_g);
				raw[col * 3 + 2] = abs(z5_r - z9_r) + abs(z6_r - z8_r);
				if (raw[col * 3] > max_b) max_b = raw[col * 3];
				if (raw[col * 3 + 1] > max_g) max_g = raw[col * 3 + 1];
				if (raw[col * 3 + 2] > max_r) max_r = raw[col * 3 + 2];
			}
			else
			{
				int z5_b = old_data[col];
				int z6_b = old_data[col + 1];
				int z8_b = old_data[col + width];
				int z9_b = old_data[col + width + 1];
				raw[col] = abs(z5_b - z9_b) + abs(z6_b - z8_b);
				if (raw[col] > max_b) max_b = raw[col];
			}
		}
	}
	for (int row = 0; row < height - 1; row++)
	{
		uchar *data = pDoc->image.ptr<uchar>(row);
		int *raw = praw + row * width * pDoc->image.channels();
		for (int col = 0; col < width - 1; col++)
		{
			if (pDoc->image.channels() == 3)
			{
				data[col * 3] = raw[col * 3] / max_b * 255;
				data[col * 3 + 1] = raw[col * 3 + 1] / max_g * 255;
				data[col * 3 + 2] = raw[col * 3 + 2] / max_r * 255;
			}
			else
			{
				data[col] = raw[col] / max_b * 255;
			}
		}
	}
	delete[] praw;
	pView->Invalidate();
}


void CSharpen::OnBnClickedPrewitt()
{
	// TODO:  在此添加控件通知处理程序代码
	CMainFrame* pMain = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	COpenCVPlatDoc *pDoc = (COpenCVPlatDoc*)pMain->GetActiveDocument();
	COpenCVPlatView *pView = (COpenCVPlatView*)pMain->GetActiveView();
	int height = pDoc->image.size().height;
	int width = pDoc->image.size().width;
	int *praw = new int[width * height * pDoc->image.channels()];
	double max_r = 0;
	double max_g = 0;
	double max_b = 0;
	for (int row = 1; row < height - 1; row++)
	{
		uchar *old_data = old_img.ptr<uchar>(row);
		int *raw = praw + row * width * pDoc->image.channels();
		for (int col = 1; col < width - 1; col++)
		{
			if (pDoc->image.channels() == 3)
			{
				int z1_b = old_data[col * 3 - width * 3 - 3];
				int z1_g = old_data[col * 3 - width * 3 - 2];
				int z1_r = old_data[col * 3 - width * 3 - 1];

				int z2_b = old_data[col * 3 - width * 3];
				int z2_g = old_data[col * 3 - width * 3 + 1];
				int z2_r = old_data[col * 3 - width * 3 + 2];

				int z3_b = old_data[col * 3 - width * 3 + 3];
				int z3_g = old_data[col * 3 - width * 3 + 4];
				int z3_r = old_data[col * 3 - width * 3 + 5];

				int z4_b = old_data[col * 3 - 3];
				int z4_g = old_data[col * 3 - 2];
				int z4_r = old_data[col * 3 - 1];

				int z6_b = old_data[col * 3 + 3];
				int z6_g = old_data[col * 3 + 4];
				int z6_r = old_data[col * 3 + 5];

				int z7_b = old_data[col * 3 + width * 3 - 3];
				int z7_g = old_data[col * 3 + width * 3 - 2];
				int z7_r = old_data[col * 3 + width * 3 - 1];

				int z8_b = old_data[col * 3 + width * 3];
				int z8_g = old_data[col * 3 + width * 3 + 1];
				int z8_r = old_data[col * 3 + width * 3 + 2];

				int z9_b = old_data[col * 3 + width * 3 + 3];
				int z9_g = old_data[col * 3 + width * 3 + 4];
				int z9_r = old_data[col * 3 + width * 3 + 5];

				raw[col * 3] = abs(z7_b + z8_b + z9_b - z1_b - z2_b - z3_b)
					+ abs(z3_b + z6_b + z9_b - z1_b - z4_b - z7_b);
				raw[col * 3 + 1] = abs(z7_g + z8_g + z9_g - z1_g - z2_g - z3_g)
					+ abs(z3_g + z6_g + z9_g - z1_g - z4_g - z7_g);
				raw[col * 3 + 2] = abs(z7_r + z8_r + z9_r - z1_r - z2_r - z3_r)
					+ abs(z3_r + z6_r + z9_r - z1_r - z4_r - z7_r);
				if (raw[col * 3] > max_b) max_b = raw[col * 3];
				if (raw[col * 3 + 1] > max_g) max_g = raw[col * 3 + 1];
				if (raw[col * 3 + 2] > max_r) max_r = raw[col * 3 + 2];
			}
			else
			{
				int z1_b = old_data[col - width - 1];
				int z2_b = old_data[col - width];
				int z3_b = old_data[col - width + 1];
				int z4_b = old_data[col - 1];
				int z6_b = old_data[col + 1];
				int z7_b = old_data[col + width - 1];
				int z8_b = old_data[col + width];
				int z9_b = old_data[col + width + 1];

				raw[col] = abs(z7_b + z8_b + z9_b - z1_b - z2_b - z3_b)
					+ abs(z3_b + z6_b + z9_b - z1_b - z4_b - z7_b);
				if (raw[col] > max_b) max_b = raw[col];
			}
		}
	}
	for (int row = 0; row < height - 1; row++)
	{
		uchar *data = pDoc->image.ptr<uchar>(row);
		int *raw = praw + row * width * pDoc->image.channels();
		for (int col = 0; col < width - 1; col++)
		{
			if (pDoc->image.channels() == 3)
			{
				data[col * 3] = raw[col * 3] / max_b * 255;
				data[col * 3 + 1] = raw[col * 3 + 1] / max_g * 255;
				data[col * 3 + 2] = raw[col * 3 + 2] / max_r * 255;
			}
			else
			{
				data[col] = raw[col] / max_b * 255;
			}
		}
	}
	delete[] praw;
	pView->Invalidate();
}


void CSharpen::OnBnClickedSobel()
{
	// TODO:  在此添加控件通知处理程序代码
	CMainFrame* pMain = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	COpenCVPlatDoc *pDoc = (COpenCVPlatDoc*)pMain->GetActiveDocument();
	COpenCVPlatView *pView = (COpenCVPlatView*)pMain->GetActiveView();
	int height = pDoc->image.size().height;
	int width = pDoc->image.size().width;
	int *praw = new int[width * height * pDoc->image.channels()];
	double max_r = 0;
	double max_g = 0;
	double max_b = 0;
	for (int row = 1; row < height - 1; row++)
	{
		uchar *old_data = old_img.ptr<uchar>(row);
		int *raw = praw + row * width * pDoc->image.channels();
		for (int col = 1; col < width - 1; col++)
		{
			if (pDoc->image.channels() == 3)
			{
				int z1_b = old_data[col * 3 - width * 3 - 3];
				int z1_g = old_data[col * 3 - width * 3 - 2];
				int z1_r = old_data[col * 3 - width * 3 - 1];

				int z2_b = old_data[col * 3 - width * 3];
				int z2_g = old_data[col * 3 - width * 3 + 1];
				int z2_r = old_data[col * 3 - width * 3 + 2];

				int z3_b = old_data[col * 3 - width * 3 + 3];
				int z3_g = old_data[col * 3 - width * 3 + 4];
				int z3_r = old_data[col * 3 - width * 3 + 5];

				int z4_b = old_data[col * 3 - 3];
				int z4_g = old_data[col * 3 - 2];
				int z4_r = old_data[col * 3 - 1];

				int z6_b = old_data[col * 3 + 3];
				int z6_g = old_data[col * 3 + 4];
				int z6_r = old_data[col * 3 + 5];

				int z7_b = old_data[col * 3 + width * 3 - 3];
				int z7_g = old_data[col * 3 + width * 3 - 2];
				int z7_r = old_data[col * 3 + width * 3 - 1];

				int z8_b = old_data[col * 3 + width * 3];
				int z8_g = old_data[col * 3 + width * 3 + 1];
				int z8_r = old_data[col * 3 + width * 3 + 2];

				int z9_b = old_data[col * 3 + width * 3 + 3];
				int z9_g = old_data[col * 3 + width * 3 + 4];
				int z9_r = old_data[col * 3 + width * 3 + 5];

				raw[col * 3] = abs(z7_b + 2*z8_b + z9_b - z1_b - 2*z2_b - z3_b)
					+ abs(z3_b + 2*z6_b + z9_b - z1_b - 2*z4_b - z7_b);
				raw[col * 3 + 1] = abs(z7_g + 2*z8_g + z9_g - z1_g - 2*z2_g - z3_g)
					+ abs(z3_g + 2*z6_g + z9_g - z1_g - 2*z4_g - z7_g);
				raw[col * 3 + 2] = abs(z7_r + 2*z8_r + z9_r - z1_r - 2*z2_r - z3_r)
					+ abs(z3_r + 2*z6_r + z9_r - z1_r - 2*z4_r - z7_r);
				if (raw[col * 3] > max_b) max_b = raw[col * 3];
				if (raw[col * 3 + 1] > max_g) max_g = raw[col * 3 + 1];
				if (raw[col * 3 + 2] > max_r) max_r = raw[col * 3 + 2];
			}
			else
			{
				int z1_b = old_data[col - width - 1];
				int z2_b = old_data[col - width];
				int z3_b = old_data[col - width + 1];
				int z4_b = old_data[col - 1];
				int z6_b = old_data[col + 1];
				int z7_b = old_data[col + width - 1];
				int z8_b = old_data[col + width];
				int z9_b = old_data[col + width + 1];

				raw[col] = abs(z7_b + 2*z8_b + z9_b - z1_b - 2*z2_b - z3_b)
					+ abs(z3_b + 2*z6_b + z9_b - z1_b - 2*z4_b - z7_b);
				if (raw[col] > max_b) max_b = raw[col];
			}
		}
	}
	for (int row = 0; row < height - 1; row++)
	{
		uchar *data = pDoc->image.ptr<uchar>(row);
		int *raw = praw + row * width * pDoc->image.channels();
		for (int col = 0; col < width - 1; col++)
		{
			if (pDoc->image.channels() == 3)
			{
				data[col * 3] = raw[col * 3] / max_b * 255;
				data[col * 3 + 1] = raw[col * 3 + 1] / max_g * 255;
				data[col * 3 + 2] = raw[col * 3 + 2] / max_r * 255;
			}
			else
			{
				data[col] = raw[col] / max_b * 255;
			}
		}
	}
	delete[] praw;
	pView->Invalidate();
}


void CSharpen::OnBnClickedLaplace3x3()
{
	// TODO:  在此添加控件通知处理程序代码
	CMainFrame* pMain = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	COpenCVPlatDoc *pDoc = (COpenCVPlatDoc*)pMain->GetActiveDocument();
	COpenCVPlatView *pView = (COpenCVPlatView*)pMain->GetActiveView();
	int height = pDoc->image.size().height;
	int width = pDoc->image.size().width;
	int *praw = new int[width * height * pDoc->image.channels()];
	double max_r = 0;
	double max_g = 0;
	double max_b = 0;
	double min_r = 0;
	double min_g = 0;
	double min_b = 0;
	for (int row = 1; row < height - 1; row++)
	{
		uchar *old_data = old_img.ptr<uchar>(row);
		int *raw = praw + row * width * pDoc->image.channels();
		for (int col = 1; col < width - 1; col++)
		{
			if (pDoc->image.channels() == 3)
			{
				int z1_b = old_data[col * 3 - width * 3 - 3];
				int z1_g = old_data[col * 3 - width * 3 - 2];
				int z1_r = old_data[col * 3 - width * 3 - 1];

				int z2_b = old_data[col * 3 - width * 3];
				int z2_g = old_data[col * 3 - width * 3 + 1];
				int z2_r = old_data[col * 3 - width * 3 + 2];

				int z3_b = old_data[col * 3 - width * 3 + 3];
				int z3_g = old_data[col * 3 - width * 3 + 4];
				int z3_r = old_data[col * 3 - width * 3 + 5];

				int z4_b = old_data[col * 3 - 3];
				int z4_g = old_data[col * 3 - 2];
				int z4_r = old_data[col * 3 - 1];

				int z5_b = old_data[col * 3];
				int z5_g = old_data[col * 3 + 1];
				int z5_r = old_data[col * 3 + 2];

				int z6_b = old_data[col * 3 + 3];
				int z6_g = old_data[col * 3 + 4];
				int z6_r = old_data[col * 3 + 5];

				int z7_b = old_data[col * 3 + width * 3 - 3];
				int z7_g = old_data[col * 3 + width * 3 - 2];
				int z7_r = old_data[col * 3 + width * 3 - 1];

				int z8_b = old_data[col * 3 + width * 3];
				int z8_g = old_data[col * 3 + width * 3 + 1];
				int z8_r = old_data[col * 3 + width * 3 + 2];

				int z9_b = old_data[col * 3 + width * 3 + 3];
				int z9_g = old_data[col * 3 + width * 3 + 4];
				int z9_r = old_data[col * 3 + width * 3 + 5];

				raw[col * 3] = 8 * z5_b - (z1_b + z2_b + z3_b + z4_b + z6_b + z7_b + z8_b + z9_b);
				raw[col * 3 + 1] = 8 * z5_g - (z1_g + z2_g + z3_g + z4_g + z6_g + z7_g + z8_g + z9_g);
				raw[col * 3 + 2] = 8 * z5_r - (z1_r + z2_r + z3_r + z4_r + z6_r + z7_r + z8_r + z9_r);
				if (raw[col * 3] > max_b) max_b = raw[col * 3];
				if (raw[col * 3 + 1] > max_g) max_g = raw[col * 3 + 1];
				if (raw[col * 3 + 2] > max_r) max_r = raw[col * 3 + 2];
				if (raw[col * 3] < min_b) min_b = raw[col * 3];
				if (raw[col * 3 + 1] < min_g) min_g = raw[col * 3 + 1];
				if (raw[col * 3 + 2] < min_r) min_r = raw[col * 3 + 2];
			}
			else
			{
				int z1_b = old_data[col - width - 1];
				int z2_b = old_data[col - width];
				int z3_b = old_data[col - width + 1];
				int z4_b = old_data[col - 1];
				int z5_b = old_data[col];
				int z6_b = old_data[col + 1];
				int z7_b = old_data[col + width - 1];
				int z8_b = old_data[col + width];
				int z9_b = old_data[col + width + 1];

				raw[col] = 8 * z5_b - (z1_b + z2_b + z3_b + z4_b + z6_b + z7_b + z8_b + z9_b);
				if (raw[col] > max_b) max_b = raw[col];
				if (raw[col] < min_b) min_b = raw[col];
			}
		}
	}
	for (int row = 0; row < height - 1; row++)
	{
		uchar *data = pDoc->image.ptr<uchar>(row);
		int *raw = praw + row * width * pDoc->image.channels();
		for (int col = 0; col < width - 1; col++)
		{
			if (pDoc->image.channels() == 3)
			{
				data[col * 3] = (raw[col * 3] - min_b) / (max_b - min_b) * 255;
				data[col * 3 + 1] = (raw[col * 3 + 1] - min_g) / (max_g - min_g) * 255;
				data[col * 3 + 2] = (raw[col * 3 + 2] - min_r) / (max_r - min_r) * 255;
			}
			else
			{
				data[col] = (raw[col] - min_b) / (max_b - min_b) * 255;
			}
		}
	}
	delete[] praw;
	pView->Invalidate();
}


void CSharpen::OnBnClickedCompositeLaplacianMask5()
{
	// TODO:  在此添加控件通知处理程序代码
	CMainFrame* pMain = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	COpenCVPlatDoc *pDoc = (COpenCVPlatDoc*)pMain->GetActiveDocument();
	COpenCVPlatView *pView = (COpenCVPlatView*)pMain->GetActiveView();
	int height = sharpened_img.size().height;
	int width = sharpened_img.size().width;
	int *praw = new int[width * height * sharpened_img.channels()];
	for (int row = 1; row < height - 1; row++)
	{
		uchar *old_data = old_img.ptr<uchar>(row);
		int *raw = praw + row * width * pDoc->image.channels();
		for (int col = 1; col < width - 1; col++)
		{
			if (sharpened_img.channels() == 3)
			{
				int z2_b = old_data[col * 3 - width * 3];
				int z2_g = old_data[col * 3 - width * 3 + 1];
				int z2_r = old_data[col * 3 - width * 3 + 2];

				int z4_b = old_data[col * 3 - 3];
				int z4_g = old_data[col * 3 - 2];
				int z4_r = old_data[col * 3 - 1];

				int z5_b = old_data[col * 3];
				int z5_g = old_data[col * 3 + 1];
				int z5_r = old_data[col * 3 + 2];

				int z6_b = old_data[col * 3 + 3];
				int z6_g = old_data[col * 3 + 4];
				int z6_r = old_data[col * 3 + 5];

				int z8_b = old_data[col * 3 + width * 3];
				int z8_g = old_data[col * 3 + width * 3 + 1];
				int z8_r = old_data[col * 3 + width * 3 + 2];

				raw[col * 3] = 5 * z5_b - (z2_b + z4_b + z6_b + z8_b);
				raw[col * 3 + 1] = 5 * z5_g - (z2_g + z4_g + z6_g + z8_g);
				raw[col * 3 + 2] = 5 * z5_r - (z2_r + z4_r + z6_r + z8_r);
			}
			else
			{
				int z2_b = old_data[col - width];;
				int z4_b = old_data[col - 1];
				int z5_b = old_data[col];
				int z6_b = old_data[col + 1];
				int z8_b = old_data[col + width];

				raw[col] = 5 * z5_b - (z2_b + z4_b + z6_b + z8_b);
			}
		}
	}
	for (int row = 0; row < height - 1; row++)
	{
		uchar *data = sharpened_img.ptr<uchar>(row);
		int *raw = praw + row * width * pDoc->image.channels();
		for (int col = 0; col < width - 1; col++)
		{
			if (sharpened_img.channels() == 3)
			{
				if (raw[col * 3] > 255) raw[col * 3] = 255;
				else if (raw[col * 3] < 0) raw[col * 3] = 0;
				data[col * 3] = raw[col * 3];
				if (raw[col * 3 + 1] > 255) raw[col * 3 + 1] = 255;
				else if (raw[col * 3 + 1] < 0) raw[col * 3 + 1] = 0;
				data[col * 3 + 1] = raw[col * 3 + 1];
				if (raw[col * 3 + 2] > 255) raw[col * 3 + 2] = 255;
				else if (raw[col * 3 + 2] < 0) raw[col * 3 + 2] = 0;
				data[col * 3 + 2] = raw[col * 3 + 2];
			}
			else
			{
				if (raw[col] > 255) raw[col] = 255;
				else if (raw[col] < 0) raw[col] = 0;
				data[col] = raw[col];
			}
		}
	}
	delete[] praw;
	scroll();
	pView->Invalidate();
}


void CSharpen::OnBnClickedCompositeLaplacianMask9()
{
	// TODO:  在此添加控件通知处理程序代码
	CMainFrame* pMain = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	COpenCVPlatDoc *pDoc = (COpenCVPlatDoc*)pMain->GetActiveDocument();
	COpenCVPlatView *pView = (COpenCVPlatView*)pMain->GetActiveView();
	int height = sharpened_img.size().height;
	int width = sharpened_img.size().width;
	int *praw = new int[width * height * sharpened_img.channels()];
	for (int row = 1; row < height - 1; row++)
	{
		uchar *old_data = old_img.ptr<uchar>(row);
		int *raw = praw + row * width * pDoc->image.channels();
		for (int col = 1; col < width - 1; col++)
		{
			if (sharpened_img.channels() == 3)
			{
				int z1_b = old_data[col * 3 - width * 3 - 3];
				int z1_g = old_data[col * 3 - width * 3 - 2];
				int z1_r = old_data[col * 3 - width * 3 - 1];

				int z2_b = old_data[col * 3 - width * 3];
				int z2_g = old_data[col * 3 - width * 3 + 1];
				int z2_r = old_data[col * 3 - width * 3 + 2];

				int z3_b = old_data[col * 3 - width * 3 + 3];
				int z3_g = old_data[col * 3 - width * 3 + 4];
				int z3_r = old_data[col * 3 - width * 3 + 5];

				int z4_b = old_data[col * 3 - 3];
				int z4_g = old_data[col * 3 - 2];
				int z4_r = old_data[col * 3 - 1];

				int z5_b = old_data[col * 3];
				int z5_g = old_data[col * 3 + 1];
				int z5_r = old_data[col * 3 + 2];

				int z6_b = old_data[col * 3 + 3];
				int z6_g = old_data[col * 3 + 4];
				int z6_r = old_data[col * 3 + 5];

				int z7_b = old_data[col * 3 + width * 3 - 3];
				int z7_g = old_data[col * 3 + width * 3 - 2];
				int z7_r = old_data[col * 3 + width * 3 - 1];

				int z8_b = old_data[col * 3 + width * 3];
				int z8_g = old_data[col * 3 + width * 3 + 1];
				int z8_r = old_data[col * 3 + width * 3 + 2];

				int z9_b = old_data[col * 3 + width * 3 + 3];
				int z9_g = old_data[col * 3 + width * 3 + 4];
				int z9_r = old_data[col * 3 + width * 3 + 5];

				raw[col * 3] = 9 * z5_b - (z1_b + z2_b + z3_b + z4_b + z6_b + z7_b + z8_b + z9_b);
				raw[col * 3 + 1] = 9 * z5_g - (z1_g + z2_g + z3_g + z4_g + z6_g + z7_g + z8_g + z9_g);
				raw[col * 3 + 2] = 9 * z5_r - (z1_r + z2_r + z3_r + z4_r + z6_r + z7_r + z8_r + z9_r);
			}
			else
			{
				int z1_b = old_data[col - width - 1];
				int z2_b = old_data[col - width];
				int z3_b = old_data[col - width + 1];
				int z4_b = old_data[col - 1];
				int z5_b = old_data[col];
				int z6_b = old_data[col + 1];
				int z7_b = old_data[col + width - 1];
				int z8_b = old_data[col + width];
				int z9_b = old_data[col + width + 1];

				raw[col] = 9 * z5_b - (z1_b + z2_b + z3_b + z4_b + z6_b + z7_b + z8_b + z9_b);
			}
		}
	}
	for (int row = 0; row < height - 1; row++)
	{
		uchar *data = sharpened_img.ptr<uchar>(row);
		int *raw = praw + row * width * pDoc->image.channels();
		for (int col = 0; col < width - 1; col++)
		{
			if (sharpened_img.channels() == 3)
			{
				if (raw[col * 3] > 255) raw[col * 3] = 255;
				else if (raw[col * 3] < 0) raw[col * 3] = 0;
				data[col * 3] = raw[col * 3];
				if (raw[col * 3 + 1] > 255) raw[col * 3 + 1] = 255;
				else if (raw[col * 3 + 1] < 0) raw[col * 3 + 1] = 0;
				data[col * 3 + 1] = raw[col * 3 + 1];
				if (raw[col * 3 + 2] > 255) raw[col * 3 + 2] = 255;
				else if (raw[col * 3 + 2] < 0) raw[col * 3 + 2] = 0;
				data[col * 3 + 2] = raw[col * 3 + 2];
			}
			else
			{
				if (raw[col] > 255) raw[col] = 255;
				else if (raw[col] < 0) raw[col] = 0;
				data[col] = raw[col];
			}
		}
	}
	delete[] praw;
	scroll();
	pView->Invalidate();
}
