#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QResource>
#include <QMessageBox>

#include "ResourceCompiler.h"
#include "rcc.h"

ResourceCompiler::ResourceCompiler()
{
//	qSetGlobalQHashSeed(0);
}

ResourceCompiler::~ResourceCompiler()
{
	UnregisterBinary();
}

bool ResourceCompiler::CompileAndRegister(const QString& sQrcSourceFilePath)
{
	UnregisterBinary();
	return Compile(sQrcSourceFilePath) && RegisterBinary();
}

QString ResourceCompiler::GetBinaryFilePath() const
{
	return m_sRccBinaryOutputFilePath;
}

QString ResourceCompiler::GetErrorText() const
{
	return m_sErrText;
}

bool ResourceCompiler::CheckIfQrcFileExists(const QString& sQssFileName, QString& sQrcFileName)
{
	const QFileInfo qss_info(sQssFileName);
	sQrcFileName = qss_info.canonicalPath() + QDir::separator()
		+ qss_info.completeBaseName() + ".qrc";
	return (QFile::exists(sQrcFileName));
}

bool ResourceCompiler::Compile(const QString& sQrcSourceFilePath)
{
	ErrDev errdev;
	errdev.open(QIODevice::WriteOnly | QIODevice::Text);

	RCCResourceLibrary rcclib(2);	// using version '2' format

	rcclib.setCompressionAlgorithm(RCCResourceLibrary::CompressionAlgorithm::Zlib);
	rcclib.setFormat(RCCResourceLibrary::Binary);
	rcclib.setInputFiles(QStringList() << sQrcSourceFilePath);

	if (!rcclib.readFiles(false, errdev))
	{
		m_sErrText = errdev.GetOutput();
		return false;
	}

	if (m_pRccBinTempFile == nullptr)
		m_pRccBinTempFile.reset(new QTemporaryFile());
	if (!m_pRccBinTempFile->open())
		return false;

	QFile temp;
	const bool bRes = rcclib.output(*m_pRccBinTempFile, temp, errdev);
	if (bRes)
	{
		m_pRccBinTempFile->flush();
		m_sRccBinaryOutputFilePath = m_pRccBinTempFile->fileName();
	}
	else
		m_sErrText = errdev.GetOutput();
	return bRes;
}

bool ResourceCompiler::RegisterBinary()
{
	return QResource::registerResource(m_sRccBinaryOutputFilePath);
}

bool ResourceCompiler::UnregisterBinary()
{
	bool bRes = true;
	if (!m_sRccBinaryOutputFilePath.isEmpty())
	{
		bRes = QResource::unregisterResource(m_sRccBinaryOutputFilePath);
		m_sRccBinaryOutputFilePath = "";
	}
	if (m_pRccBinTempFile != nullptr)
	{
//		if (m_pRccBinTempFile->isOpen())
//			m_pRccBinTempFile->close();
		m_pRccBinTempFile.reset(nullptr);
	}

	return bRes;
}

ResourceCompiler::ErrDev::ErrDev()
{
	m_Data.reserve(1024);
}

QString ResourceCompiler::ErrDev::GetOutput() const
{
	return QString(m_Data);
}

qint64 ResourceCompiler::ErrDev::readData(char* data, qint64 maxlen)
{
	return 0;
}

qint64 ResourceCompiler::ErrDev::writeData(const char* data, qint64 len)
{
	m_Data.append(data, len);
	return len;
}
