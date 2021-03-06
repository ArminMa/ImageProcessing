#pragma once

#include <opencv2\opencv.hpp>
// CSaturation 对话框

class CSaturation : public CDialogEx
{
	DECLARE_DYNAMIC(CSaturation)

public:
	CSaturation(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSaturation();

// 对话框数据
	enum { IDD = IDD_DIALOG_SATURATION };
	cv::Mat old_labImg;
	cv::Mat labImg;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};
