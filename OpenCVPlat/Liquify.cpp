// Liquify.cpp : 实现文件
//

#include "stdafx.h"
#include "OpenCVPlat.h"
#include "Liquify.h"
#include "afxdialogex.h"
#include "MainFrm.h"
#include "OpenCVPlatDoc.h"
#include "OpenCVPlatView.h"

// CLiquify 对话框

IMPLEMENT_DYNAMIC(CLiquify, CDialogEx)

CLiquify::CLiquify(CWnd* pParent /*=NULL*/)
: CDialogEx(CLiquify::IDD, pParent)
{
	mouse_down = false;
}

CLiquify::~CLiquify()
{
	CI.Destroy();
	if (!CI_ori.IsNull())
		CI_ori.Destroy();
}

void CLiquify::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CLiquify, CDialogEx)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_OKAY, &CLiquify::OnBnClickedOkay)
	ON_BN_CLICKED(IDC_CANCEL, &CLiquify::OnBnClickedCancel)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


// CLiquify 消息处理程序


BOOL CLiquify::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CSliderCtrl *pSlidCtrl = (CSliderCtrl*)GetDlgItem(IDC_BRUSH_SIZE);
	pSlidCtrl->SetRange(10, 100);
	pSlidCtrl->SetPos(10);
	pSlidCtrl = (CSliderCtrl*)GetDlgItem(IDC_BRUSH_STRENGTH);
	pSlidCtrl->SetRange(10, 100);
	pSlidCtrl->SetPos(10);
	CMainFrame* pMain = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	COpenCVPlatDoc *pDoc = (COpenCVPlatDoc*)pMain->GetActiveDocument();
	cv::Mat &image = pDoc->image;
	if (image.cols > 0 && image.rows > 0)
	{
		CWnd *pic_ctrl = GetDlgItem(IDC_IMAGE);
		int w = image.cols;//宽
		int h = image.rows;//高
		int channels = image.channels();//通道数 

		CI.Create(w, h, 8 * channels);
		if (channels == 1)
		{
			RGBQUAD* ColorTable;
			int MaxColors = 256;
			ColorTable = new RGBQUAD[MaxColors];
			CI.GetColorTable(0, MaxColors, ColorTable);
			for (int i = 0; i<MaxColors; i++)
			{
				ColorTable[i].rgbBlue = (BYTE)i;
				ColorTable[i].rgbGreen = (BYTE)i;
				ColorTable[i].rgbRed = (BYTE)i;
			}
			CI.SetColorTable(0, MaxColors, ColorTable);
			delete[]ColorTable;
		}

		uchar *pS;
		uchar *pImg = (uchar *)CI.GetBits();
		int step = CI.GetPitch();
		for (int i = 0; i < h; i++)
		{
			pS = image.ptr<uchar>(i);
			memcpy(pImg + i*step, pS, sizeof(uchar)* channels * w);
		}
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}


void CLiquify::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值

	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CLiquify::OnBnClickedOkay()
{
	// TODO:  在此添加控件通知处理程序代码
	CMainFrame* pMain = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	COpenCVPlatDoc *pDoc = (COpenCVPlatDoc*)pMain->GetActiveDocument();
	COpenCVPlatView *pView = (COpenCVPlatView*)pMain->GetActiveView();
	cv::Mat &image = pDoc->image;
	int nChannels = CI.GetBPP() / 8;
	int nWidth = CI.GetWidth();
	int nHeight = CI.GetHeight();
	uchar* pucRow;
	uchar* pucImage = (uchar*)CI.GetBits();
	int nStep = CI.GetPitch();
	for (int nRow = 0; nRow < nHeight; nRow++)
	{
		pucRow = (image.ptr<uchar>(nRow));
		for (int nCol = 0; nCol < nWidth; nCol++)
		{
			if (1 == nChannels)
			{
				pucRow[nCol] = *(pucImage + nRow * nStep + nCol);
			}
			else if (3 == nChannels)
			{
				for (int nCha = 0; nCha < 3; nCha++)
				{
					pucRow[nCol * 3 + nCha] = *(pucImage + nRow * nStep + nCol * 3 + nCha);
				}
			}
		}
	}
	pView->Invalidate();
	EndDialog(IDOK);
}


void CLiquify::OnBnClickedCancel()
{
	// TODO:  在此添加控件通知处理程序代码
	EndDialog(IDCANCEL);
}


void CLiquify::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO:  在此处添加消息处理程序代码
	if (!CI.IsNull())
	{
		CWnd *pic_ctrl = GetDlgItem(IDC_IMAGE);
		CRect rect;
		pic_ctrl->GetWindowRect(&rect);
		ScreenToClient(&rect);
		CDC *pDC = pic_ctrl->GetDC();
		HRGN rgn = CreateRectRgn(0, 0, MIN(CI.GetWidth(), rect.right - rect.left),
			MIN(CI.GetHeight(), rect.bottom - rect.top));
		SelectClipRgn(*pDC, rgn);
		CI.Draw(*pic_ctrl->GetDC(), 0, 0);
		ReleaseDC(pic_ctrl->GetDC());
	}
	// 不为绘图消息调用 CDialogEx::OnPaint()
}


void CLiquify::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	if (!CI.IsNull())
	{
		CWnd *pic_ctrl = GetDlgItem(IDC_IMAGE);
		CSliderCtrl *pSlidCtrl = (CSliderCtrl*)GetDlgItem(IDC_BRUSH_SIZE);
		int brush_size = pSlidCtrl->GetPos();
		int &r = brush_size;
		pSlidCtrl = (CSliderCtrl*)GetDlgItem(IDC_BRUSH_STRENGTH);
		int brush_strength = pSlidCtrl->GetPos();
		CRect rect;
		pic_ctrl->GetWindowRect(&rect);
		ScreenToClient(&rect);
		CDC *pDC = pic_ctrl->GetDC();
		HRGN rgn = CreateRectRgn(0, 0, MIN(CI.GetWidth(), rect.right - rect.left),
			MIN(CI.GetHeight(), rect.bottom - rect.top));
		SelectClipRgn(*pDC, rgn);
		if (PtInRect(&rect, point))
		{
			int x = point.x - rect.left;
			int y = point.y - rect.top;

			int last_x = last_point.x - rect.left;
			int last_y = last_point.y - rect.top;
			if (mouse_down)
			{
				int channels = CI.GetBPP() / 8;
				uchar *pImg = (uchar *)CI_ori.GetBits();
				int step = CI_ori.GetPitch();
				uchar *buff = new uchar[4 * channels * r * r];
				for (int i = last_y - r; i < last_y + r; i++)
				for (int j = last_x - r; j < last_x + r; j++)
				{
					if (i >= 0 && i < CI.GetHeight()
						&& j >= 0 && j < CI.GetWidth())
					{
						double src_x;
						double src_y;
						iiw(r, brush_strength, CPoint(last_x, last_y), CPoint(x, y),
							j, i, &src_x, &src_y);
						for (int k = last_points.size() - 1; k >= 1; k--)
						{
							double jt = src_x;
							double it = src_y;
							int xs = last_points[k].x - rect.left;
							int ys = last_points[k].y - rect.top;
							int last_xs = last_points[k - 1].x - rect.left;
							int last_ys = last_points[k - 1].y - rect.top;
							iiw(r, brush_strength, CPoint(last_xs, last_ys), CPoint(xs, ys),
								jt, it, &src_x, &src_y);
						}
						int pos = (i - last_y + r) * 2 * channels * r + (j - last_x + r) * channels;
						if (src_x >= 0 && src_x < CI_ori.GetWidth() - 1
							&& src_y >= 0 && src_y < CI_ori.GetHeight() - 1)
						{
							int x1 = src_x;
							int y1 = src_y;
							double t = src_x - x1;
							double u = src_y - y1;

							if (channels == 3)
							{
								uchar *p = pImg + step * y1 + x1 * channels;
								uchar b1 = *p;
								uchar g1 = *(p + 1);
								uchar r1 = *(p + 2);
								uchar b2 = *(p + 3);
								uchar g2 = *(p + 4);
								uchar r2 = *(p + 5);
								p = pImg + step * (y1 + 1) + x1 * channels;
								uchar b3 = *p;
								uchar g3 = *(p + 1);
								uchar r3 = *(p + 2);
								uchar b4 = *(p + 3);
								uchar g4 = *(p + 4);
								uchar r4 = *(p + 5);

								buff[pos] = (1 - t)*(1 - u)*b1 + t*(1 - u)*b2 + (1 - t)*u*b3 + t*u*b4;
								buff[pos + 1] = (1 - t)*(1 - u)*g1 + t*(1 - u)*g2 + (1 - t)*u*g3 + t*u*g4;
								buff[pos + 2] = (1 - t)*(1 - u)*r1 + t*(1 - u)*r2 + (1 - t)*u*r3 + t*u*r4;
							}
							else
							{
								uchar *p = pImg + step * y1 + x1 * channels;
								uchar b1 = *p;
								uchar b2 = *(p + 1);
								p = pImg + step * (y1 + 1) + x1 * channels;
								uchar b3 = *p;
								uchar b4 = *(p + 1);

								buff[pos] = (1 - t)*(1 - u)*b1 + t*(1 - u)*b2 + (1 - t)*u*b3 + t*u*b4;
							}
						}
						else
						{
							if (channels == 3)
							{
								buff[pos] = 0;
								buff[pos + 1] = 0;
								buff[pos + 2] = 0;
							}
							else
							{
								buff[pos] = 0;
							}
						}
					}
				}
				pImg = (uchar *)CI.GetBits();
				step = CI.GetPitch();
				for (int i = last_y - r; i < last_y + r; i++)
				for (int j = last_x - r; j < last_x + r; j++)
				{
					if (i >= 0 && i < CI.GetHeight()
						&& j >= 0 && j < CI.GetWidth())
					{
						uchar *p = pImg + step * i + j * channels;
						int pos = (i - last_y + r) * 2 * channels * r + (j - last_x + r) * channels;
						*p = buff[pos];
						if (channels == 3)
						{
							*(p + 1) = buff[pos + 1];
							*(p + 2) = buff[pos + 2];
						}
					}
				}
				delete[] buff;
				last_points.push_back(point);
			}
			CI.Draw(*pDC, 0, 0);
			CBrush* pOldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);
			pDC->Ellipse(x - r, y - r, x + r, y + r);
			pDC->SelectObject(pOldBrush);
		}
		last_point = point;
	}
	CDialogEx::OnMouseMove(nFlags, point);
}


void CLiquify::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	mouse_down = true;
	if (!CI.IsNull())
	{
		CI_ori.Create(CI.GetWidth(), CI.GetHeight(), CI.GetBPP());
		if (CI.GetBPP() <= 8 && CI.IsIndexed())
		{
			RGBQUAD pal[256];
			int nColors = CI.GetMaxColorTableEntries();
			if (nColors>0)
			{
				CI.GetColorTable(0, nColors, pal);
				CI_ori.SetColorTable(0, nColors, pal);
			}
		}
		uchar *destPtr = (uchar*)CI_ori.GetBits();
		int destPitch = CI_ori.GetPitch();
		uchar *srcPtr = (uchar*)CI.GetBits();
		int srcPitch = CI.GetPitch();
		for (int i = 0; i<CI.GetHeight(); i++)
		{
			memcpy(destPtr + i*destPitch, srcPtr + i*srcPitch, abs(srcPitch));
		}
	}
	CDialogEx::OnLButtonDown(nFlags, point);
}


void CLiquify::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	mouse_down = false;
	if (!CI.IsNull())
	{
		last_points.clear();
		if (!CI_ori.IsNull())
			CI_ori.Destroy();
	}
	CDialogEx::OnLButtonUp(nFlags, point);
}

void iiw(int radius, int strength, CPoint last, CPoint current, double point_x, double point_y,
	double *src_x, double *src_y)
{
	double rs = radius;
	double xc = sqrt((point_x - last.x)*(point_x - last.x)
		+ (point_y - last.y)*(point_y - last.y));
	if (rs < xc)
	{
		*src_x = point_x;
		*src_y = point_y;
	}
	else
	{
		double tb = xc / rs;
		double tc = strength/50.0 * (3 * tb*tb*tb*tb - 4 * tb*tb*tb + 1);
		*src_x = point_x - tc * (current.x - last.x);
		*src_y = point_y - tc * (current.y - last.y);
	}
}
