#include <QApplication>
#include <QDebug>

#include "qexcel.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	//���ļ���ȡ�ù�����
    QExcel j("C:/Users/min/Desktop/QExcel/QExcelDemo/source.xls");

	//ȡ�ù���������
    // qDebug() << "SheetCount" << j.getSheetsCount();

	//����һ�Ź�����
	//j.selectSheet(1);
    //j.selectSheet("JSheet2");

	//ȡ�ù���������
	//j.selectSheet(1);
	//j.getSheetName();
    //qDebug()<<"SheetName 1"<<j.getSheetName(1);

	//ȡ�ù�������ʹ�÷�Χ
	//int topLeftRow, topLeftColumn, bottomRightRow, bottomRightColumn;
	//j.getUsedRange(&topLeftRow, &topLeftColumn, &bottomRightRow, &bottomRightColumn);

	//��ֵ
	//j.getCellValue(2, 2).toString();

	//ɾ��������
	//j.selectSheet("Sheet1");
	//j.selectSheet(1);
	//j.deleteSheet();
	//j.save();

	//��������
    //j.selectSheet("JSheet3");
    //j.setCellString(1, 7, "addString");
    //j.setCellString("A3", "abc");
    //j.save();

	//�ϲ���Ԫ��
	//j.selectSheet(2);
	//j.mergeCells("G1:H2");
	//j.mergeCells(4, 7, 5 ,8);
	//j.save();

	//�����п�
	//j.selectSheet(1);
	//j.setColumnWidth(1, 20);
	//j.save();

	//���ô���
	//j.selectSheet(1);
	//j.setCellFontBold(2, 2, true);
	//j.setCellFontBold("A2", true);
	//j.save();

	//�������ִ�С
	//j.selectSheet(1);
	//j.setCellFontSize("B3", 20);
	//j.setCellFontSize(1, 2, 20);
	//j.save();

	//���õ�Ԫ�����־���
	//j.selectSheet(2);
	//j.setCellTextCenter(1, 2);
	//j.setCellTextCenter("A2");
	//j.save();

	//���õ�Ԫ�������Զ�����
	//j.selectSheet(1);
	//j.setCellTextWrap(2,2,true);
	//j.setCellTextWrap("A2", true);
	//j.save();

	//����һ������Ӧ�и�
	//j.selectSheet(1);
	//j.setAutoFitRow(2);
	//j.save();

	//�½�������
	//j.insertSheet("abc");
	//j.save();

	//�����Ԫ������
	//j.selectSheet(4);
	//j.clearCell(1,1);
    //j.clearCell("A2");
    //j.save();

	//�ϲ�һ������ͬ�����ĵ�Ԫ��
	//j.selectSheet(1);
	//j.mergeSerialSameCellsInColumn(1, 2);
	//j.save();

	//��ȡһ�Ź�������������
    j.selectSheet(1);
    qDebug() << j.getUsedRowsCount();

	//�����и�
//	j.selectSheet(1);
//    j.setRowHeight(2, 200);
//	j.save();

	j.close();
	qDebug()<<"App End";
	return a.exec();
}
