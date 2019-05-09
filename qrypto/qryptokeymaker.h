/* Qrypto 2019
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** Botan 1.11 is licensed under Simplified BSD License
** CryptoPP 5.6.2 is licensed under Boost Software License 1.0
**/
#ifndef QRYPTO_KEYMAKER_H
#define QRYPTO_KEYMAKER_H

#include "qrypto.h"
#include "sequre.h"

namespace Qrypto
{

/**
 * @brief The KeyMaker class conforms to PKCS #5 PBKDF2-HMAC
 * @ref https://tools.ietf.org/html/rfc2898#section-5.2
 */
class KeyMaker
{
    struct Impl;
    friend struct Impl;

    QString m_algorithmName;
    SequreData m_key;
    QByteArray m_salt;
    uint m_iteration;
    uint m_iterationTime;

public:
    enum Algorithm {
        RipeMD_160,
        RipeMD_320,
        Sha1,
        Sha224,
        Sha256,
        Sha384,
        Sha512,
        Sha3_224,
        Sha3_256,
        Sha3_384,
        Sha3_512,
        Tiger,
        Whirlpool,
        UnknownAlgorithm
    };

    static const QStringList AlgorithmNames;

    /**
     * @brief KeyMaker default constructor
     * @param algorithm
     * @param keyLength in bytes
     */
    KeyMaker(Algorithm algorithm = Sha256, uint keyLength = 16) :
        m_algorithmName(AlgorithmNames.at(algorithm)),
        m_key(keyLength, '\0'),
        m_iteration(100000),
        m_iterationTime(0)
    { }

    /**
     * @brief authenticate message using HMAC of current Algorithm with internal key
     * @param messageData
     * @param messageSize in bytes
     * @param truncatedSize in bytes of digest code
     * @return digest code or null QByteArray on error
     * @ref https://tools.ietf.org/html/rfc2898#section-7.1
     */
    QByteArray authenticate(const char *messageData, uint messageSize, uint truncatedSize = 0) const;

    QByteArray authenticate(const QByteArray &message, uint truncatedSize = 0) const
    { return authenticate(message.constData(), message.size(), truncatedSize); }

    /**
     * @brief deriveKey generates internal key
     * @param passwordData should not be null
     * @param passwordSize should not be zero
     * @param keyLength in bytes, will use existing key length if zero
     * @return derivation error
     */
    Error deriveKey(const char *passwordData, uint passwordSize, uint keyLength = 0);

    Error deriveKey(const QByteArray &password, uint keyLength = 0)
    { return deriveKey(password.constData(), password.size(), keyLength); }

    const uchar *keyData() const
    { return m_key->data(); }

    Algorithm algorithm() const
    {
        for (int i = AlgorithmNames.size(); i-- > 0; ) {
            if (AlgorithmNames.at(i).compare(m_algorithmName, Qt::CaseInsensitive) == 0)
                return Algorithm(i);
        }

        return UnknownAlgorithm;
    }

    void setAlgorithm(Algorithm algorithm)
    { m_algorithmName = AlgorithmNames.at(algorithm); }

    QString algorithmName() const
    { return m_algorithmName; }

    /**
     * @brief setAlgorithmName
     * @param algorithmName will be matched Caseinsensitively
     */
    void setAlgorithmName(const QString &algorithmName)
    {
        if (AlgorithmNames.contains(algorithmName, Qt::CaseInsensitive))
            m_algorithmName = algorithmName;
        else
            m_algorithmName.clear();
    }

    /**
     * @brief iterationCount is 100000 by default
     * @return
     */
    uint iterationCount() const
    { return m_iteration; }

    void setIterationCount(uint iterationCount)
    { m_iteration = iterationCount; }

    /**
     * @brief iterationTime is 0 milliseconds by default (disabled)
     * @return
     */
    uint iterationTime() const
    { return m_iterationTime; }

    void setIterationTime(uint milliseconds)
    { m_iterationTime = milliseconds; }

    uint keyBitSize() const
    { return keyLength() * 8; }

    void setKeyBitSize(uint bits)
    { setKeyLength(bits / 8); }

    uint keyLength() const
    { return m_key->size(); }

    void setKeyLength(uint keyLength)
    { m_key.resize(keyLength); }

    /**
     * @brief salt will be autogenerated to half of hash digest size in deriveKey if it was empty
     * @return
     */
    QByteArray salt() const
    { return m_salt; }

    /**
     * @brief setSalt would be used for decryption
     * @param salt
     * @warning a salt value of all zeroes is unusable, key maker will regenerate
     */
    void setSalt(const QByteArray &salt)
    { m_salt = salt; }

    void setSalt(const QString &saltHex)
    { setSalt(QByteArray::fromHex(saltHex.toLatin1())); }

    void setSaltLength(int saltLength)
    { setSalt(QByteArray(saltLength, '\0')); }
};

}

#endif // QRYPTO_KEYMAKER_H
