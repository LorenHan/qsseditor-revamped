#ifndef RESOURCECOMPILER_H
#define RESOURCECOMPILER_H

#include <QIODevice>
#include <QString>
#include <QScopedPointer>
#include <QTemporaryFile>

class ResourceCompiler
{
public:
	ResourceCompiler();
	~ResourceCompiler();

	bool CompileAndRegister(const QString& sQrcSourceFilePath);
	bool UnregisterBinary();

	QString GetBinaryFilePath() const;
	QString GetErrorText() const;

	static bool CheckIfQrcFileExists(const QString& sQssFileName, QString& sQrcFileName);

private:
	bool Compile(const QString& sQrcSourceFilePath);
	bool RegisterBinary();

private:
	class ErrDev : public QIODevice
	{
	public:
		ErrDev();

		QString GetOutput() const;

		// QIODevice interface
	protected:
		qint64 readData(char* data, qint64 maxlen) override;
		qint64 writeData(const char* data, qint64 len) override;

	private:
		QByteArray m_Data;
	};

private:
	QString m_sRccBinaryOutputFilePath;
	QScopedPointer<QTemporaryFile> m_pRccBinTempFile;
	QString m_sErrText;
};

#endif // RESOURCECOMPILER_H
