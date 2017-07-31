/**
 *  Copyright (c) 1999~2017, Altibase Corp. and/or its affiliates. All rights reserved.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License, version 3,
 *  as published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <ulnSetConnectAttr.h>
#include <ulnURL.h>
#include <ulnString.h>
#include <ulnConnString.h>
#include <ulnFailOver.h>
#include <ulnDbc.h>
/* PROJ-1573 XA */
#include <ulxXaConnection.h>
#include <ulnDataSource.h>

/*
 * ULN_SFID_82
 * SQLSetConnectAttr(), DBC, C2
 *      -- [1]
 *      (08003) [2]
 *  where
 *      [1] The Attribute argument was not SQL_ATTR_TRANSLATE_LIB or SQL_ATTR_TRANSLATE_OPTION.
 *      [2] The Attribute argument was SQL_ATTR_TRANSLATE_LIB or SQL_ATTR_TRANSLATE_OPTION.
 */

ACI_RC ulnSFID_82(ulnFnContext *aFnContext)
{
    acp_sint32_t sAttribute;

    if (aFnContext->mWhere == ULN_STATE_ENTRY_POINT)
    {
        sAttribute = *(acp_sint32_t *)(aFnContext->mArgs);
        switch (sAttribute)
        {
            case SQL_ATTR_TRANSLATE_LIB:
            case SQL_ATTR_TRANSLATE_OPTION:  /* [2] : 08003 */
                ACI_RAISE(LABEL_ABORT_NO_CONN);
                break;

            default:;
        }
    }

    return ACI_SUCCESS;

    ACI_EXCEPTION(LABEL_ABORT_NO_CONN)
    {
        ulnError(aFnContext, ulERR_ABORT_NO_CONNECTION, "");
    }
    ACI_EXCEPTION_END;

    return ACI_FAILURE;
}

/*
 * ULN_SFID_83
 * SQLSetConnectAttr(), DBC, C4, C5
 *      --[3]
 *      (08002)[4]
 *      HY011[5]
 *  where
 *      [3] The Attribute argument was not SQL_ATTR_ODBC_CURSORS or SQL_ATTR_PACKET_SIZE.
 *      [4] The Attribute argument was SQL_ATTR_ODBC_CURSORS.
 *      [5] The Attribute argument was SQL_ATTR_PACKET_SIZE.
 */
ACI_RC ulnSFID_83(ulnFnContext *aFnContext)
{
    acp_sint32_t sAttribute;

    if (aFnContext->mWhere == ULN_STATE_ENTRY_POINT)
    {
        sAttribute = *(acp_sint32_t *)(aFnContext->mArgs);
        switch ( sAttribute )
        {
            case SQL_ATTR_PORT:
            case ALTIBASE_NLS_USE:
            case ALTIBASE_NLS_NCHAR_LITERAL_REPLACE: /* PROJ-1579 NCHAR */
            case ALTIBASE_NLS_CHARACTERSET_VALIDATION: /* PROJ-1579 NCHAR */
            case ALTIBASE_NLS_CHARACTERSET: /* PROJ-1579 NCHAR */
            case ALTIBASE_NLS_NCHAR_CHARACTERSET: /* PROJ-1579 NCHAR */
            case SQL_ATTR_ODBC_CURSORS:  /* [4] : 08002 */
            case ALTIBASE_PREFER_IPV6:   /* proj-1538 ipv6 */
                ACI_RAISE(LABEL_ABORT_CONNECTION);
                break;

            case ALTIBASE_XA_NAME:
            case ALTIBASE_XA_RMID:
            case SQL_ATTR_PACKET_SIZE:  /* [5] : HY011 */
            case SQL_ATTR_CURRENT_CATALOG:
                ACI_RAISE(LABEL_ABORT_SET);
                break;

            default:
                break;
        }
    }

    return ACI_SUCCESS;

    ACI_EXCEPTION(LABEL_ABORT_CONNECTION)
    {
        ulnError(aFnContext, ulERR_ABORT_CONNECTION_NAME_IN_USE);
    }
    ACI_EXCEPTION(LABEL_ABORT_SET)
    {
        ulnError(aFnContext, ulERR_ABORT_ATTRIBUTE_CANNOT_BE_SET_NOW);
    }
    ACI_EXCEPTION_END;

    return ACI_FAILURE;
}

/*
 * ULN_SFID_84
 * SQLSetConnectAttr(), DBC ���� �����Լ� : C6 ����
 *      --[3] and [6]
 *      C5[8]
 *      (08002)[4]
 *      HY011[5] or [7]
 *  where
 *      [3] The Attribute argument was not SQL_ATTR_ODBC_CURSORS or SQL_ATTR_PACKET_SIZE.
 *      [4] The Attribute argument was SQL_ATTR_ODBC_CURSORS.
 *      [5] The Attribute argument was SQL_ATTR_PACKET_SIZE.
 *      [6] The Attribute argument was not SQL_ATTR_AUTOCOMMIT,
 *          or the Attribute argument was SQL_ATTR_AUTOCOMMIT and setting this attribute
 *          did not commit the transaction.
 *      [7] The Attribute argument was SQL_ATTR_TXN_ISOLATION.
 *      [8] The Attribute argument was SQL_ATTR_AUTOCOMMIT,
 *          and setting this attribute committed the transaction.
 */
ACI_RC ulnSFID_84(ulnFnContext *aFnContext)
{
    acp_sint32_t sAttribute;

    if (aFnContext->mWhere == ULN_STATE_ENTRY_POINT)
    {
        sAttribute = *(acp_sint32_t *)(aFnContext->mArgs);

        /* ONLY BEFORE CONNECT */
        switch (sAttribute)
        {
            case SQL_ATTR_PORT:
            case SQL_ATTR_ODBC_CURSORS: /* [4] : 08002 */
                ACI_RAISE(LABEL_CONN_NAME_IN_USE);
                break;

            case ALTIBASE_XA_NAME:
            case ALTIBASE_XA_RMID:
            case ALTIBASE_NLS_USE:
            case ALTIBASE_NLS_NCHAR_LITERAL_REPLACE:
            case ALTIBASE_NLS_CHARACTERSET_VALIDATION: /* PROJ-1579 NCHAR */
            case ALTIBASE_NLS_CHARACTERSET: /* PROJ-1579 NCHAR */
            case ALTIBASE_NLS_NCHAR_CHARACTERSET: /* PROJ-1579 NCHAR */
            case SQL_ATTR_FAILOVER:
            case SQL_BATCH_SUPPORT:
            case SQL_ATTR_CONNECTION_POOLING:
            case SQL_ATTR_PACKET_SIZE: /* [5] or [7] : HY011 */
            case ALTIBASE_PREFER_IPV6: /* proj-1538 ipv6     */
            case SQL_ATTR_CURRENT_CATALOG:
                ACI_RAISE(LABEL_INVALID_TIME);
                break;

            default:
                break;
        }
    }

    return ACI_SUCCESS;

    ACI_EXCEPTION(LABEL_INVALID_TIME)
    {
        ulnError(aFnContext, ulERR_ABORT_ATTRIBUTE_CANNOT_BE_SET_NOW);
    }

    ACI_EXCEPTION(LABEL_CONN_NAME_IN_USE)
    {
        ulnError(aFnContext, ulERR_ABORT_CONNECTION_NAME_IN_USE);
    }

    ACI_EXCEPTION_END;

    return ACI_FAILURE;
}

/* BUG-39817 */
static ACI_RC ulnSetTxnIsolation(ulnFnContext *aFnContext, void *aValuePtr)
{
    acp_slong_t   sValue = (acp_slong_t)aValuePtr;
    ulnDbc       *sDbc;

    ULN_FNCONTEXT_GET_DBC(aFnContext, sDbc);
    ACE_ASSERT( sDbc != NULL );

    switch (sValue)
    {
        case ULN_ISOLATION_LEVEL_DEFAULT: /* 0 */
        case SQL_TXN_READ_UNCOMMITTED:    /* 1 */
        case SQL_TXN_READ_COMMITTED:      /* 2 */
        case SQL_TXN_REPEATABLE_READ:     /* 4 */
        case SQL_TXN_SERIALIZABLE:        /* 8 */
            /* acceptable */
            break;

        default:
            ACI_RAISE(LABEL_INVALID_ATTR_VALUE);
            break;
    }

    if (sValue == SQL_TXN_READ_UNCOMMITTED)
    {
        ulnError(aFnContext, ulERR_IGNORE_OPTION_VALUE_CHANGED,
                 "SQL_ATTR_TXN_ISOLATION changed to SQL_TXN_READ_COMMITTED");
        sValue = SQL_TXN_READ_COMMITTED;
    }
    else
    {
        /* nothing to do */
    }

    if (ulnDbcIsConnected(sDbc) == ACP_TRUE)
    {
        if (sValue == ULN_ISOLATION_LEVEL_DEFAULT)
        {
            ACI_RAISE(LABEL_INVALID_ATTR_VALUE);
        }
        else
        {
            /* nothing to do */
        }

        /* send to server */
        ACI_TEST(ulnSendConnectAttr(aFnContext,
                                    ULN_PROPERTY_ISOLATION_LEVEL,
                                    (void *)sValue) != ACI_SUCCESS);
    }
    else
    {
        /* will be set after connect, if disconnected */
    }

    aFnContext->mHandle.mDbc->mAttrTxnIsolation = (acp_uint32_t)sValue;

    return ACI_SUCCESS;

    ACI_EXCEPTION(LABEL_INVALID_ATTR_VALUE);
    {
        ulnError(aFnContext, ulERR_ABORT_INVALID_ATTRIBUTE_VALUE);
    }

    ACI_EXCEPTION_END;

    return ACI_FAILURE;
}

static acp_sint32_t ulnAttrLength(void *aValuePtr, acp_sint32_t aLength)
{
    switch (aLength)
    {
        case SQL_NULL_DATA:               //(-1)
            aLength = 0;
            break;

        case SQL_DATA_AT_EXEC:            //(-2)
            break;

        case SQL_NTS:                     //(-3)
            aLength = (acp_sint32_t)acpCStrLen((acp_char_t *)aValuePtr, ACP_SINT32_MAX);
            break;

        case SQL_IS_POINTER:              //(-4)
            aLength = ACI_SIZEOF(void *);
            break;

        case SQL_IS_UINTEGER:             //(-5)
        case SQL_IS_INTEGER :             //(-6)
            aLength = ACI_SIZEOF(acp_uint32_t);
            break;

        case SQL_IS_USMALLINT:            //(-7)
        case SQL_IS_SMALLINT:             //(-8)
            aLength = ACI_SIZEOF(acp_uint16_t);
            break;

        default:
            if (aLength < SQL_LEN_BINARY_ATTR_OFFSET)
            {
                aLength = (-aLength) + SQL_LEN_BINARY_ATTR_OFFSET;
            }
            break;
    }

    return aLength;
}

static ACI_RC ulnSetConnAttrConnType(ulnFnContext *aFnContext, ulnConnType aConnType)
{
    ulnDbc *sDbc = aFnContext->mHandle.mDbc;

    if (ulnDbcGetConnType(sDbc) == ULN_CONNTYPE_INVALID) // �����Ǿ����� ����
    {
        // �����Ǿ� ���� ���� ��쿡�� ����

        switch (aConnType)
        {
            case ULN_CONNTYPE_TCP:
                ACI_TEST_RAISE(ulnDbcSetCmiLinkImpl(sDbc, CMI_LINK_IMPL_TCP) != ACI_SUCCESS,
                               LABEL_NOT_SUPPORTED_LINK);
                break;

            case ULN_CONNTYPE_UNIX:
                ACI_TEST_RAISE(ulnDbcSetCmiLinkImpl(sDbc, CMI_LINK_IMPL_UNIX) != ACI_SUCCESS,
                               LABEL_NOT_SUPPORTED_LINK);
                break;

            case ULN_CONNTYPE_IPC:
                ACI_TEST_RAISE(ulnDbcSetCmiLinkImpl(sDbc, CMI_LINK_IMPL_IPC) != ACI_SUCCESS,
                               LABEL_NOT_SUPPORTED_LINK);
                break;

            case ULN_CONNTYPE_IPCDA:
                ACI_TEST_RAISE(ulnDbcSetCmiLinkImpl(sDbc, CMI_LINK_IMPL_IPCDA) != ACI_SUCCESS,
                               LABEL_NOT_SUPPORTED_LINK);
                break;

            case ULN_CONNTYPE_SSL:
                /* PROJ-2474 SSL/TLS */
                ACI_TEST_RAISE(ulnDbcSetCmiLinkImpl(sDbc, CMI_LINK_IMPL_SSL) != ACI_SUCCESS, 
                               LABEL_NOT_SUPPORTED_LINK);
                break;

            case ULN_CONNTYPE_INVALID:
                ACI_RAISE(LABEL_INVALID_CONN_TYPE_ERR);
                break;
        }

        ulnDbcSetConnType(sDbc, aConnType);
    }

    return ACI_SUCCESS;

    ACI_EXCEPTION(LABEL_NOT_SUPPORTED_LINK)
    {
        ulnError(aFnContext, ulERR_ABORT_CONNTYPE_NOT_SUPPORTED, aConnType);
    }
    ACI_EXCEPTION(LABEL_INVALID_CONN_TYPE_ERR)
    {
        /*
         * HY010
         */
        ulnError(aFnContext, ulERR_ABORT_INVALID_CONN_TYPE_ERROR, aConnType);
    }
    ACI_EXCEPTION_END;

    return ACI_FAILURE;
}

static ACI_RC ulnSetConnAttrPortNumber(ulnFnContext *aFnContext, acp_sint32_t aPortNumber)
{
    ulnDbc *sDbc = aFnContext->mHandle.mDbc;

    // BUG-38108 Cannot reuse SQLHDBC after connection failure
    ulnDbcSetPortNumber(sDbc, aPortNumber);

    return ACI_SUCCESS;
}

static ACI_RC ulnSetConnAttrCheckValue(ulnFnContext  *aFnContext,
                                       ulnConnAttrID  aConnAttrID,
                                       void          *aValuePtr,
                                       acp_sint32_t   aLength)
{
    acp_sint32_t        sMaxLength;
    ulnDiscreteInt     *sAllowedDIntList;
    ulnReservedString  *sAllowedStrList;
    ulnMetricPrefixInt *sAllowedMIntList;

    /*
     * BUGBUG : �� �Լ��� ������ ��� String Ÿ�� �Ӽ� ������ aValuePtr ��
     *          null terminated �� ����Ǿ�� �Ѵ�.
     */

    acp_slong_t sValue = (acp_slong_t)aValuePtr;

    /*
     * ���� �˻� �� ���� üũ, �׸��� optional feature not implemented üũ
     */
    switch (gUlnConnAttrTable[aConnAttrID].mAttrType)
    {
        case ULN_CONN_ATTR_TYPE_INT:

            sAllowedMIntList = (ulnMetricPrefixInt *)gUlnConnAttrTable[aConnAttrID].mCheck;
            ACI_TEST_RAISE(sAllowedMIntList == NULL, LABEL_NOT_IMPLEMENTED);

            ACI_TEST_RAISE((sValue < gUlnConnAttrTable[aConnAttrID].mMinValue) ||
                           (sValue > gUlnConnAttrTable[aConnAttrID].mMaxValue),
                           LABEL_INVALID_ATTR_VALUE);
            break;


        case ULN_CONN_ATTR_TYPE_DISCRETE_INT:

            sAllowedDIntList = (ulnDiscreteInt *)gUlnConnAttrTable[aConnAttrID].mCheck;
            ACI_TEST_RAISE(sAllowedDIntList == NULL, LABEL_NOT_IMPLEMENTED);

            while (sAllowedDIntList->mKey != NULL)
            {
                if (sAllowedDIntList->mValue == sValue)
                {
                    break;
                }

                sAllowedDIntList++;
            }

            ACI_TEST_RAISE(sAllowedDIntList->mKey == NULL, LABEL_INVALID_ATTR_VALUE);
            break;


        case ULN_CONN_ATTR_TYPE_STRING:
        case ULN_CONN_ATTR_TYPE_UPPERCASE_STRING:

            sAllowedStrList = (ulnReservedString *)gUlnConnAttrTable[aConnAttrID].mCheck;
            ACI_TEST_RAISE(sAllowedStrList == NULL, LABEL_NOT_IMPLEMENTED);

            if (gUlnConnAttrTable[aConnAttrID].mMaxValue == 0)
            {
                sMaxLength = ACP_SINT32_MAX;
            }
            else
            {
                sMaxLength = gUlnConnAttrTable[aConnAttrID].mMaxValue;
            }

            ACI_TEST_RAISE((aLength < gUlnConnAttrTable[aConnAttrID].mMinValue) ||
                           (aLength > sMaxLength),
                           LABEL_INVALID_ATTR_LENGTH);
            break;

        case ULN_CONN_ATTR_TYPE_CALLBACK:
        case ULN_CONN_ATTR_TYPE_POINTER:
            break;

        default:
            ACE_ASSERT(0);
            break;
    }

    return ACI_SUCCESS;

    ACI_EXCEPTION(LABEL_INVALID_ATTR_VALUE);
    {
        /* HY024 */
        ulnError(aFnContext, ulERR_ABORT_INVALID_ATTRIBUTE_VALUE);
    }

    ACI_EXCEPTION(LABEL_NOT_IMPLEMENTED)
    {
        /* HYC00 : Optional feature not implemented */
        ulnError(aFnContext, ulERR_ABORT_OPTIONAL_FEATURE_NOT_IMPLEMENTED);
    }

    ACI_EXCEPTION(LABEL_INVALID_ATTR_LENGTH);
    {
        /* HY024 */
        ulnError(aFnContext, ulERR_ABORT_STRING_LENGTH_TOO_LONG);
    }

    ACI_EXCEPTION_END;

    return ACI_FAILURE;
}

static ACI_RC ulnSetConnAttr(ulnFnContext  *aFnContext,
                             ulnConnAttrID  aConnAttrID,
                             void          *aValuePtr,
                             acp_sint32_t   aLength)
{
    /*
     * BUGBUG : �� �Լ��� ������ ��� String Ÿ�� �Ӽ� ������ aValuePtr ��
     *          null terminated �� ����Ǿ�� �Ѵ�.
     */

    acp_slong_t  sValue = (acp_slong_t)aValuePtr;
    ulnDbc      *sDbc   = aFnContext->mHandle.mDbc;
    acp_bool_t   sIsXaConnection = ulnIsXaConnection(sDbc);
    acp_char_t   sNlsUse[20];
    acp_sint32_t sNlsUseLen;

    /*
     * ���� �˻� �� ���� üũ, �׸��� optional feature not implemented üũ
     */

    ACI_TEST(ulnSetConnAttrCheckValue(aFnContext, aConnAttrID, aValuePtr, aLength)
             != ACI_SUCCESS);

    /* 
     * BUG-36256 Improve property's communication
     *
     * �������� �������� �ʴ� ������Ƽ�� ������ ��û�� ���� �ʴ´�.
     * �� On�� �ص� �׻� Off �ȴ�.
     */
    ACI_TEST_RAISE(ulnConnAttrArrHasId(&sDbc->mUnsupportedProperties,
                                       aConnAttrID) == ACI_SUCCESS,
                   NO_NEED_WORK);

    switch (aConnAttrID)
    {
        /*
         * =================================================================
         * Connection String �̳� ODBC.INI �� ���ؼ��� ������ �� �ִ� �Ӽ���
         * =================================================================
         */

        /*
         * --------------------------------------
         * String �Ӽ��� : null üũ�� �̹� �̷������
         * --------------------------------------
         */

        case ULN_CONN_ATTR_DSN:
            ACI_TEST_RAISE(ulnDbcSetDsnString(sDbc, (acp_char_t *)aValuePtr, aLength) != ACI_SUCCESS,
                           LABEL_NOT_ENOUGH_MEMORY);
            break;

        case ULN_CONN_ATTR_UID:
            ACI_TEST_RAISE(ulnDbcSetUserName(sDbc, (acp_char_t *)aValuePtr, aLength) != ACI_SUCCESS,
                           LABEL_NOT_ENOUGH_MEMORY);
            break;

        case ULN_CONN_ATTR_UNIXDOMAIN_FILEPATH:
            ACI_TEST_RAISE(ulnDbcSetUnixdomainFilepath(sDbc, (acp_char_t *)aValuePtr, aLength) != ACI_SUCCESS,
                           LABEL_NOT_ENOUGH_MEMORY);
            break;

        case ULN_CONN_ATTR_IPC_FILEPATH:
            ACI_TEST_RAISE(ulnDbcSetIpcFilepath(sDbc, (acp_char_t *)aValuePtr, aLength) != ACI_SUCCESS,
                           LABEL_NOT_ENOUGH_MEMORY);
            break;

        case ULN_CONN_ATTR_PWD:
            ACI_TEST_RAISE(ulnDbcSetPassword(sDbc, (acp_char_t *)aValuePtr, aLength) != ACI_SUCCESS,
                           LABEL_NOT_ENOUGH_MEMORY);
            break;

        case ULN_CONN_ATTR_HOSTNAME:
            ACI_TEST_RAISE(ulnDbcSetHostNameString(sDbc, (acp_char_t *)aValuePtr, aLength) != ACI_SUCCESS,
                           LABEL_NOT_ENOUGH_MEMORY);
            break;

        /*
         * ---------------------------------------
         * acp_uint32_t, Bool �Ӽ���
         * ---------------------------------------
         */

        case ULN_CONN_ATTR_CONNTYPE:
            ACI_TEST(ulnSetConnAttrConnType(aFnContext, (ulnConnType)sValue) != ACI_SUCCESS);
            break;

        // bug-19279 remote sysdba enable
        // ex) PRIVILEGE=SYSDBA
        case ULN_CONN_ATTR_PRIVILEGE:
            ulnDbcSetPrivilege(sDbc, (ulnPrivilege)sValue);
            break;

        //PROJ-1645 UL Failover.
        case ULN_CONN_ATTR_ALTERNATE_SERVERS:
            ACI_TEST_RAISE(ulnDbcSetAlternateServers(sDbc, (acp_char_t *)aValuePtr, aLength) != ACI_SUCCESS,
                           LABEL_NOT_ENOUGH_MEMORY);
            break;
        case ULN_CONN_ATTR_CONNECTION_RETRY_COUNT:
            ulnDbcSetConnectionRetryCount(sDbc,(acp_uint32_t)sValue);
            break;
        case ULN_CONN_ATTR_CONNECTION_RETRY_DELAY:
            ulnDbcSetConnectionRetryDelay(sDbc,(acp_uint32_t)sValue);
            break;
        case ULN_CONN_ATTR_SESSION_FAILOVER:
            ulnDbcSetSessionFailover(sDbc,(acp_bool_t)sValue);
            break;

        /* bug-31468: adding conn-attr for trace logging */
        case ULN_CONN_ATTR_TRACELOG:
            ulnDbcSetTraceLog(sDbc,(acp_uint32_t)sValue);
            break;

        /*
         * =================================================================================
         * Connection String, ODBC.INI, SQLSetConnectAttr ��������ε� ������ �� �ִ� �Ӽ���
         * =================================================================================
         */

        /*
         * --------------------------------------
         * String �Ӽ���
         * --------------------------------------
         */

        case ULN_CONN_ATTR_NLS_USE:
            /* ------------------------------------------------
             *  1. �̹� ����� �����̰ų�,
             *  2. �ʱ� ����String Ȥ�� DSN ���� ���� �о�� �� �ʱ������ ���
             *     ���� ���������ϴ�.
             *
             *     ===> ulnSendConnectAttr() �� ȣ������ �ʴ´�.
             *          (PROJ-1579 NCHAR���� ȣ���ϴ� ������ �����)
             * ----------------------------------------------*/
            ACI_TEST_RAISE(aValuePtr == NULL, ERR_HY090);

            acpCStrCpy(sNlsUse, ACI_SIZEOF(sNlsUse), (acp_char_t*)aValuePtr, aLength);
            sNlsUseLen = (acp_sint32_t)acpCStrLen(sNlsUse, ACI_SIZEOF(sNlsUse));

            acpCStrToUpper(sNlsUse, sNlsUseLen);

            //BUG-22684
            // Client Characterset��  mClientCharsetLangModule�� �����Ѵ�.
            ACI_TEST_RAISE(mtlModuleByName((const mtlModule **)&(sDbc->mClientCharsetLangModule),
                                           sNlsUse,
                                           sNlsUseLen)
                           != ACI_SUCCESS, LABEL_CHARACTERSET_NOT_FOUND);

            // bug-26661: nls_use not applied to nls module for ut
            // nls_use ���� UT���� ����� gNlsModuleForUT���� ����
            ACI_TEST_RAISE(mtlModuleByName((const mtlModule **)&gNlsModuleForUT,
                                           sNlsUse,
                                           sNlsUseLen)
                           != ACI_SUCCESS, LABEL_CHARACTERSET_NOT_FOUND);

            ACI_TEST_RAISE(ulnDbcSetNlsLangString(sDbc, (acp_char_t *)sNlsUse, sNlsUseLen) != ACI_SUCCESS,
                           LABEL_NOT_ENOUGH_MEMORY);

            // PROJ-1579 NCHAR
            // ������ ALTIBASE_NLS_USE(Ŭ���̾�Ʈ ĳ���� ��) ������ �˷��ش�.
            ACI_TEST(ulnSendConnectAttr(aFnContext, ULN_PROPERTY_NLS, sDbc->mNlsLangString)
                     != ACI_SUCCESS);
            break;

        case ULN_CONN_ATTR_DATE_FORMAT:
            ACI_TEST_RAISE(aValuePtr == NULL, ERR_HY090);
            ACI_TEST_RAISE(ulnDbcSetDateFormat(sDbc, (acp_char_t *)aValuePtr, aLength) != ACI_SUCCESS,
                           LABEL_NOT_ENOUGH_MEMORY);

            ACI_TEST(ulnSendConnectAttr(aFnContext, ULN_PROPERTY_DATE_FORMAT, sDbc->mDateFormat)
                     != ACI_SUCCESS);
            break;

        case ULN_CONN_ATTR_TIME_ZONE:
            ACI_TEST_RAISE( aValuePtr == NULL, ERR_HY090 );
            ACI_TEST_RAISE( ulnDbcSetTimezoneSring( sDbc, (acp_char_t *)aValuePtr, aLength )
                            != ACI_SUCCESS,
                            LABEL_NOT_ENOUGH_MEMORY );

            ACI_TEST( ulnSendConnectAttr( aFnContext, ULN_PROPERTY_TIME_ZONE, sDbc->mTimezoneString )
                      != ACI_SUCCESS );
            break;

        case ULN_CONN_ATTR_APP_INFO:
            ACI_TEST_RAISE(aValuePtr == NULL, ERR_HY090);
            ACI_TEST_RAISE(ulnDbcSetAppInfo(sDbc, (acp_char_t *)aValuePtr, aLength) != ACI_SUCCESS,
                           LABEL_NOT_ENOUGH_MEMORY);

            ACI_TEST(ulnSendConnectAttr(aFnContext, ULN_PROPERTY_APP_INFO, sDbc->mAppInfo)
                     != ACI_SUCCESS);
            break;

        // PROJ-1579 NCHAR
        case ULN_CONN_ATTR_NLS_CHARACTERSET:
            ACI_TEST_RAISE(aValuePtr == NULL, ERR_HY090);
            break;

        // PROJ-1579 NCHAR
        case ULN_CONN_ATTR_NLS_NCHAR_CHARACTERSET:
            ACI_TEST_RAISE(aValuePtr == NULL, ERR_HY090);
            break;

        /*
         * --------------------------------
         * Integer, discrete integer �Ӽ���
         * --------------------------------
         */

        case ULN_CONN_ATTR_QUERY_TIMEOUT:
            ACI_TEST(ulnSendConnectAttr(aFnContext, ULN_PROPERTY_QUERY_TIMEOUT, (void *)sValue)  != ACI_SUCCESS);
            sDbc->mAttrQueryTimeout = (acp_uint32_t)sValue;
            break;

        case ULN_CONN_ATTR_LOGIN_TIMEOUT:
            //BUG-24574 [ODBC] SQLSetConnectAttr(dbc, SQL_ATTR_LOGIN_TIMEOUT, 0, 0); ������ ���� ����
            if (sValue != 0)
            {
                sDbc->mAttrLoginTimeout = (acp_uint32_t)sValue;
            }
            else
            {
                sDbc->mAttrLoginTimeout = ULN_TMOUT_MAX;
            }
            break;

        case ULN_CONN_ATTR_AUTOCOMMIT:
#ifdef COMPILE_SHARDCLI
            ACI_TEST_RAISE(ulsdModuleSetAutoCommit(aFnContext,
                                                   sDbc,
                                                   aValuePtr,
                                                   aLength),
                           LABEL_CANNOT_BE_SET_NOW);
#endif /* COMPILE_SHARDCLI */
            /* if xa enlist error */
            ACI_TEST_RAISE(ulnIsXaActive(sDbc) == ACP_TRUE, LABEL_CANNOT_BE_SET_NOW);
            ACI_TEST(ulnSendConnectAttr(aFnContext, ULN_PROPERTY_AUTOCOMMIT, (void *)sValue) != ACI_SUCCESS);
            sDbc->mAttrAutoCommit = (acp_uint8_t)sValue;
            break;

        case ULN_CONN_ATTR_CONNECTION_TIMEOUT:
            ulnDbcSetConnectionTimeout(sDbc, (acp_uint32_t)sValue);
            break;

        case ULN_CONN_ATTR_PACKET_SIZE:
            ulnError(aFnContext, ulERR_IGNORE_OPTION_VALUE_CHANGED,
                     "SQL_ATTR_PACKET_SIZE");
            sDbc->mAttrPacketSize = (acp_uint32_t)sValue;
            break;

        case ULN_CONN_ATTR_TXN_ISOLATION:
            ACI_TEST(ulnSetTxnIsolation(aFnContext, aValuePtr) != ACI_SUCCESS);
            break;

        case ULN_CONN_ATTR_ODBC_VERSION:
            sDbc->mOdbcVersion = (acp_uint32_t)sValue;
            break;

        case ULN_CONN_ATTR_DISCONNECT_BEHAVIOR:
            sDbc->mAttrDisconnect = (acp_uint8_t)sValue;
            break;

        case ULN_CONN_ATTR_CONNECTION_POOLING:
            /*
             * BUGBUG : ENV �� ������ �°� �����ؾ� �Ѵ�.
             */
            sDbc->mAttrConnPooling = (acp_uint8_t)sValue;
            break;

        case ULN_CONN_ATTR_PORT_NO:
            ACI_TEST(ulnSetConnAttrPortNumber(aFnContext, (acp_sint32_t)sValue) != ACI_SUCCESS);
            break;

        case ULN_CONN_ATTR_ASYNC_ENABLE:
            sDbc->mAttrAsyncEnable = (acp_uint8_t)sValue;
            break;

        case ULN_CONN_ATTR_METADATA_ID:
            break;

        case ULN_CONN_ATTR_EXPLAIN_PLAN:
            ACI_TEST(ulnSendConnectAttr(aFnContext, ULN_PROPERTY_EXPLAIN_PLAN, (void *)sValue)
                     != ACI_SUCCESS);
            sDbc->mAttrExplainPlan = (acp_uint8_t)sValue;
            break;

        case ULN_CONN_ATTR_STACK_SIZE:
            ACI_TEST(ulnSendConnectAttr(aFnContext, ULN_PROPERTY_STACK_SIZE, (void *)sValue)
                     != ACI_SUCCESS);
            sDbc->mAttrStackSize = (acp_uint32_t)sValue;
            break;

        case ULN_CONN_ATTR_ACCESS_MODE:
            sDbc->mAttrAccessMode = (acp_uint32_t)sValue;
            break;

        case ULN_CONN_ATTR_OPTIMIZER_MODE:
            ACI_TEST(ulnSendConnectAttr(aFnContext,
                                        ULN_PROPERTY_OPTIMIZER_MODE,
                                        (void *)sValue) != ACI_SUCCESS);
            sDbc->mAttrOptimizerMode = (acp_uint32_t)sValue;
            break;

        /* BUG-32885 Timeout for DDL must be distinct to query_timeout or utrans_timeout */
        case ULN_CONN_ATTR_DDL_TIMEOUT:
            ACI_TEST(ulnSendConnectAttr(aFnContext, ULN_PROPERTY_DDL_TIMEOUT, (void *)sValue)
                     != ACI_SUCCESS);
            sDbc->mAttrDdlTimeout = (acp_uint32_t)sValue;
            break;

        case ULN_CONN_ATTR_UTRANS_TIMEOUT:
            ACI_TEST(ulnSendConnectAttr(aFnContext, ULN_PROPERTY_UTRANS_TIMEOUT, (void *)sValue)
                     != ACI_SUCCESS);
            sDbc->mAttrUtransTimeout = (acp_uint32_t)sValue;
            break;

        case ULN_CONN_ATTR_FETCH_TIMEOUT:
            ACI_TEST(ulnSendConnectAttr(aFnContext, ULN_PROPERTY_FETCH_TIMEOUT, (void *)sValue)
                     != ACI_SUCCESS);
            sDbc->mAttrFetchTimeout = (acp_uint32_t)sValue;
            break;

        case ULN_CONN_ATTR_IDLE_TIMEOUT:
            ACI_TEST(ulnSendConnectAttr(aFnContext, ULN_PROPERTY_IDLE_TIMEOUT, (void *)sValue)
                     != ACI_SUCCESS);
            sDbc->mAttrIdleTimeout = (acp_uint32_t)sValue;
            break;

        case ULN_CONN_ATTR_XA_RMID:
            /* PROJ-1573 XA */
            /* if dbc is already enlisted in xa, error */
            ACI_TEST_RAISE(sIsXaConnection == ACP_TRUE, LABEL_CANNOT_BE_SET_NOW);
            /* if no such rmid connection exist, error */
            ACI_TEST_RAISE(ulxFindConnection((acp_sint32_t)sValue, &(sDbc->mXaConnection))
                           != ACI_SUCCESS, LABEL_CANNOT_BE_SET_NOW);
            /* set enlist flag */
            sDbc->mXaEnlist = ACP_TRUE;
            break;

        case ULN_CONN_ATTR_XA_NAME:
            ACI_TEST_RAISE(ulnDbcSetXaName(sDbc, (acp_char_t *)aValuePtr, aLength) != ACI_SUCCESS,
                           LABEL_NOT_ENOUGH_MEMORY);
            break;

        /* BUG-20898 */
        case  ULN_CONN_ATTR_XA_LOG_DIR:
            // do Nothing
            break;

        case ULN_CONN_ATTR_ENLIST_IN_DTC:
        case ULN_CONN_ATTR_ENLIST_IN_XA:
            if (sValue == SQL_TRUE)
            {
                /* if this connection already enlisted in xa, error */
                ACI_TEST_RAISE(sIsXaConnection == ACP_TRUE,
                               LABEL_CANNOT_BE_SET_NOW);
                /* get first connection in xa connection list */
                ACI_TEST_RAISE(ulxGetFirstConnection(&sDbc->mXaConnection),
                               LABEL_CANNOT_BE_SET_NOW);
                /* set enlist flag */
                sDbc->mXaEnlist = ACP_TRUE;
            }
            else
            {
                /* if this connection already enlisted in xa, error */
                ACI_TEST_RAISE(sIsXaConnection == ACP_TRUE,
                               LABEL_CANNOT_BE_SET_NOW);
            }
            break;

        case ULN_CONN_ATTR_HEADER_DISPLAY_MODE:
            ACI_TEST(ulnSendConnectAttr(aFnContext,
                                        ULN_PROPERTY_HEADER_DISPLAY_MODE,
                                        (void *)sValue) != ACI_SUCCESS);
            sDbc->mAttrHeaderDisplayMode = (acp_uint32_t)sValue;
            break;

        case ULN_CONN_ATTR_LONGDATA_COMPAT:
            if (sValue == SQL_TRUE)
            {
                ulnDbcSetLongDataCompat(sDbc, ACP_TRUE);
            }
            else
            {
                ulnDbcSetLongDataCompat(sDbc, ACP_FALSE);
            }
            break;

        case ULN_CONN_ATTR_ANSI_APP:
            if (sValue == SQL_TRUE)
            {
                ulnDbcSetAnsiApp(sDbc, ACP_TRUE);
            }
            else
            {
                ulnDbcSetAnsiApp(sDbc, ACP_FALSE);
            }
            break;

        // PROJ-1579 NCHAR
        case ULN_CONN_ATTR_NLS_NCHAR_LITERAL_REPLACE:
            ACI_TEST(ulnSendConnectAttr(aFnContext,
                                        ULN_PROPERTY_NLS_NCHAR_LITERAL_REPLACE,
                                        (void *)sValue) != ACI_SUCCESS);
            sDbc->mNlsNcharLiteralReplace = (acp_uint32_t)sValue;
            break;

        case ULN_CONN_ATTR_NLS_CHARACTERSET_VALIDATION:
            sDbc->mNlsCharactersetValidation = (acp_uint32_t)sValue;
            break;
        /*
         * ====================================================
         * SQLSetConnectAttr() �� ���ؼ��� ���������� �Ӽ���
         * ====================================================
         */

        case ULN_CONN_ATTR_MESSAGE_CALLBACK:
            ulnRegisterServerMessageCallback(sDbc, (ulnServerMessageCallbackStruct *)aValuePtr);
            break;
        //PROJ-1645 UL Failover.
        case ULN_CONN_ATTR_FAILOVER_CALLBACK:
            ulnDbcSetFailoverCallbackContext(sDbc,(SQLFailOverCallbackContext*) aValuePtr);
            break;

        /*
         * =====================================================================
         * ���� �������� �ʾҰų� �������� �ʴ� �Ӽ��� : ���ʿ��� ���� �������.
         * =====================================================================
         */

        case ULN_CONN_ATTR_URL:
            break;
        case ULN_CONN_ATTR_ODBC_CURSORS: /* odbc 3.0 ������ �� �� ����. stmt �Ӽ� */
            sDbc->mAttrOdbcCursors = (acp_uint32_t)sValue;
            break;

        case ULN_CONN_ATTR_CURRENT_CATALOG:
            ACI_TEST_RAISE(ulnDbcSetCurrentCatalog(sDbc, (acp_char_t *)aValuePtr, aLength) != ACI_SUCCESS, 
                           LABEL_NOT_ENOUGH_MEMORY);
            break;

        case ULN_CONN_ATTR_TRACEFILE:
            ACI_TEST_RAISE(ulnDbcSetTracefile(sDbc, (acp_char_t *)aValuePtr, aLength) != ACI_SUCCESS, 
                           LABEL_NOT_ENOUGH_MEMORY);
            break;

        case ULN_CONN_ATTR_TRANSLATE_LIB:
            ACI_TEST_RAISE(ulnDbcSetTranslateLib(sDbc, (acp_char_t *)aValuePtr, aLength) != ACI_SUCCESS, 
                           LABEL_NOT_ENOUGH_MEMORY);
            break;

        case ULN_CONN_ATTR_TRANSLATE_OPTION:
            sDbc->mAttrTranslateOption = (acp_uint32_t)sValue;
            break;

        case ULN_CONN_ATTR_TRACE:
            sDbc->mAttrTrace = (acp_uint32_t)sValue;
            break;

        case ULN_CONN_ATTR_QUIET_MODE:
            sDbc->mAttrQuietMode = sValue;
            break;

        /* proj-1538 ipv6 */
        case ULN_CONN_ATTR_PREFER_IPV6:
            sDbc->mAttrPreferIPv6 = (sValue == SQL_TRUE)? ACP_TRUE: ACP_FALSE;
            break;

        /* BUG-31144 */
        case ULN_CONN_ATTR_MAX_STATEMENTS_PER_SESSION:
            ACI_TEST(ulnSendConnectAttr(aFnContext, ULN_PROPERTY_MAX_STATEMENTS_PER_SESSION, (void *)sValue)
                     != ACI_SUCCESS);
            sDbc->mAttrMaxStatementsPerSession= (acp_uint32_t)sValue;
            break;

        /* PROJ-1891 Deferred Prepare */
        case ULN_CONN_ATTR_DEFERRED_PREPARE:
            ulnDbcSetAttrDeferredPrepare(sDbc, (acp_uint8_t)sValue);
            break;

        /* PROJ-2047 Strengthening LOB - LOBCACHE */
        case ULN_CONN_ATTR_LOB_CACHE_THRESHOLD:
            ACI_TEST(ulnSendConnectAttr(aFnContext, ULN_PROPERTY_LOB_CACHE_THRESHOLD, (void *)sValue)
                     != ACI_SUCCESS);
            sDbc->mAttrLobCacheThreshold = (acp_uint32_t)sValue;
            break;

        /* BUG-36548 Return code of client functions should be differed by ODBC version */
        case ULN_CONN_ATTR_ODBC_COMPATIBILITY:
            sDbc->mAttrOdbcCompatibility = (acp_uint32_t)sValue;
            break;
                
        /* BUG-36729 Connection attribute will be added to unlock client mutex by force */
        case ULN_CONN_ATTR_FORCE_UNLOCK:
            sDbc->mAttrForceUnlock = (acp_uint32_t)sValue;
            break;

        /* PROJ-2474 SSL/TLS */
        case ULN_CONN_ATTR_SSL_CA:
            ACI_TEST_RAISE(ulnDbcSetSslCa(sDbc, (acp_char_t *)aValuePtr, aLength) != ACI_SUCCESS,
            		       LABEL_NOT_ENOUGH_MEMORY);
            break;

        case ULN_CONN_ATTR_SSL_CAPATH:
            ACI_TEST_RAISE(ulnDbcSetSslCaPath(sDbc, (acp_char_t *)aValuePtr, aLength) != ACI_SUCCESS,
            		       LABEL_NOT_ENOUGH_MEMORY);
            break;

        case ULN_CONN_ATTR_SSL_CERT:
            ACI_TEST_RAISE(ulnDbcSetSslCert(sDbc, (acp_char_t *)aValuePtr, aLength) != ACI_SUCCESS,
            		       LABEL_NOT_ENOUGH_MEMORY);
            break;

        case ULN_CONN_ATTR_SSL_KEY:
            ACI_TEST_RAISE(ulnDbcSetSslKey(sDbc, (acp_char_t *)aValuePtr, aLength) != ACI_SUCCESS,
            		       LABEL_NOT_ENOUGH_MEMORY);
            break;

        case ULN_CONN_ATTR_SSL_VERIFY:
            sDbc->mAttrSslVerify = (acp_bool_t)sValue;
            break;

        case ULN_CONN_ATTR_SSL_CIPHER:
            ACI_TEST_RAISE(ulnDbcSetSslCipher(sDbc, (acp_char_t *)aValuePtr, aLength) != ACI_SUCCESS,
            		       LABEL_NOT_ENOUGH_MEMORY);
            break;

        case ULN_CONN_ATTR_IPCDA_FILEPATH:
            ACI_TEST_RAISE(ulnDbcSetIPCDAFilepath(sDbc, (acp_char_t *)aValuePtr, aLength) != ACI_SUCCESS,
                           LABEL_NOT_ENOUGH_MEMORY);
            break;

        /* PROJ-2625 Semi-async Prefetch, Prefetch Auto-tuning */
        case ULN_CONN_ATTR_SOCK_RCVBUF_BLOCK_RATIO:
            ACI_TEST(ulnDbcSetAttrSockRcvBufBlockRatio(aFnContext,
                                                       (acp_uint32_t)sValue)
                    != ACI_SUCCESS);
            break;

        /* BUG-44271 */
        case ULN_CONN_ATTR_SOCK_BIND_ADDR:
            ACI_TEST_RAISE(ulnDbcSetSockBindAddr(sDbc, (acp_char_t *)aValuePtr, aLength) != ACI_SUCCESS,
                           LABEL_NOT_ENOUGH_MEMORY);
            break;

        /*
         * Warning : ������ �߰��Ǵ� ����-Ŭ���̾�Ʈ ������Ƽ�� �ݵ��
         *           ulnSetConnectAttrOff()�� �߰��Ǿ�� �Ѵ�.
         */

        /* PROJ-2638 shard native linker */
        case ULN_CONN_ATTR_SHARD_LINKER_TYPE:
            sDbc->mShardDbcCxt.mShardLinkerType = (acp_uint8_t)sValue;
            break;

        /* PROJ-2638 shard native linker */
        case ULN_CONN_ATTR_SHARD_NODE_NAME:
            ACI_TEST_RAISE( aLength > ULSD_MAX_NODE_NAME_LEN, LABEL_NOT_ENOUGH_MEMORY );
            acpMemCpy( sDbc->mShardDbcCxt.mShardTargetDataNodeName,
                       (acp_char_t *) aValuePtr,
                       aLength );
            sDbc->mShardDbcCxt.mShardTargetDataNodeName[aLength] = '\0';
            break;

        default:
            if (aConnAttrID >= ULN_CONN_ATTR_MAX)
            {
                ACE_ASSERT(0);
            }
            else
            {
                /* nothing to do */
            }
            break;

        /*
         * Note : compiler warning �� ���� �ʾƾ� �Ѵ�. ���� warning ���� ���� �ϳ� ��������.
         */
    }

    ACI_EXCEPTION_CONT(NO_NEED_WORK)

    return ACI_SUCCESS;

    ACI_EXCEPTION(LABEL_CANNOT_BE_SET_NOW)
    {
        ulnError(aFnContext, ulERR_ABORT_ATTRIBUTE_CANNOT_BE_SET_NOW);
    }
    ACI_EXCEPTION(ERR_HY090);
    {
        ulnError(aFnContext, ulERR_ABORT_Invalid_StrLen_IndPtr_Error);
    }
    ACI_EXCEPTION(LABEL_NOT_ENOUGH_MEMORY)
    {
        ulnError(aFnContext, ulERR_FATAL_MEMORY_ALLOC_ERROR, "ulnSetConnAttr");
    }
    ACI_EXCEPTION(LABEL_CHARACTERSET_NOT_FOUND)
    {
        ulnError(aFnContext, ulERR_ABORT_idnSetDefaultFactoryError, (acp_char_t *)aValuePtr);
    }

    ACI_EXCEPTION_END;

    return ACI_FAILURE;
}


/*
 * Note :
 *      ulnSetConnAttrByConnString() ���� ȣ��Ǵ� ulnSetConnAttrById
 *          NTS guarantee    : OK. �׻� NTS
 *          aAttrValueLength : �׻� string �� ����. ��� Ȥ�� 0
 *
 *      ulnSetConnAttrByProfileFunc() ���� ȣ��Ǵ� ulnSetConnAttrById
 *          NTS guarantee    : ?. NTS ���� �ƴ��� odbc �������� �̾߱� ����.
 *          aAttrValueLength : �׻� string �� ����. ���. ??
 */
/*
 * BUGBUG : ulnConnAttrProcess*** �Լ�����
 *              1. aAttrValue �� NULL �����Ͱ� �ȵ��´ٴ� ��.
 *              2. aAttrValue �� NTS �� ���´ٴ� ��
 *              3. aAttrValueLength �� ����� ��ȿ�� ���� ���´ٴ� ��
 *          �� �ݵ�� ������ ��.
 */
static ACI_RC ulnConnAttrProcessIntAttr(ulnFnContext  *aFnContext,
                                        ulnConnAttrID  aConnAttr,
                                        acp_char_t    *aAttrValue,
                                        acp_sint32_t   aAttrValueLength)
{
    ulnMetricPrefixInt *sAllowedMultiplicator;
    acp_sint32_t        sPosLen;
    acp_char_t         *sCurr = aAttrValue;
    acp_sint32_t        sSign             = 1;
    acp_uint64_t        sSel              = 0;
    ulvSLen             sValue;

    /*
     * BUG-28980 [CodeSonar]Null Pointer Dereference
     */
    ACI_TEST_RAISE((aAttrValue == NULL) || (aAttrValueLength < 0), LABEL_INVALID_ATTRIBUTE_VALUE);

    ACI_TEST_RAISE(acpCStrToInt64(aAttrValue,
                                  aAttrValueLength,
                                  &sSign,
                                  &sSel,
                                  10,
                                  &sCurr) != ACP_RC_SUCCESS,
                   LABEL_INVALID_ATTRIBUTE_VALUE);

    sValue = (ulvSLen)sSel * sSign; /* BUG-44151 */

    /* K, M, G, sec, hours�� ���� ���� ��ȯ ���� ó�� */
    sPosLen = ulnParseIndexOfNonWhitespace(aAttrValue, aAttrValueLength, (acp_sint32_t)(sCurr - aAttrValue));
    if (sPosLen < aAttrValueLength)
    {
        sCurr = aAttrValue + sPosLen;
        sPosLen = aAttrValueLength - sPosLen;

        sAllowedMultiplicator = (ulnMetricPrefixInt *)gUlnConnAttrTable[aConnAttr].mCheck;
        ACI_TEST_RAISE(sAllowedMultiplicator == NULL, LABEL_NOT_IMPLEMENTED);

        for (; sAllowedMultiplicator->mKey != NULL; sAllowedMultiplicator++)
        {
            if (acpCStrCmp(sAllowedMultiplicator->mKey, sCurr, sPosLen) == 0 &&
                sAllowedMultiplicator->mKey[sPosLen] == '\0')
            {
                break;
            }
        }
        ACI_TEST_RAISE(sAllowedMultiplicator->mKey == NULL, LABEL_INVALID_ATTRIBUTE_VALUE);

        sValue *= sAllowedMultiplicator->mValue;
    }

    /*
     * Note : ���� üũ�� ulnSetConnAttr ���� �Ѵ�.
     */

    /*
     * BUG-28980 [CodeSonar]strncpy Length Unreasonable
     * aAttrValueLength�� ���� ���� ������ ��� ���Ÿ� ����
     */
    ACI_TEST(ulnSetConnAttr(aFnContext,
                            aConnAttr,
                            (void *)(acp_slong_t)sValue,
                            ACI_SIZEOF(acp_slong_t)) != ACI_SUCCESS);

    return ACI_SUCCESS;

    ACI_EXCEPTION(LABEL_NOT_IMPLEMENTED)
    {
        /* HYC00 : Optional feature not implemented */
        ulnError(aFnContext, ulERR_ABORT_OPTIONAL_FEATURE_NOT_IMPLEMENTED);
    }

    ACI_EXCEPTION(LABEL_INVALID_ATTRIBUTE_VALUE)
    {
        /*
         * HY024 : Invalid attribute value
         */
        ulnError(aFnContext, ulERR_ABORT_INVALID_ATTRIBUTE_VALUE);
    }

    ACI_EXCEPTION_END;

    return ACI_FAILURE;
}

static ACI_RC ulnConnAttrProcessDiscreteIntAttr(ulnFnContext  *aFnContext,
                                                ulnConnAttrID  aConnAttr,
                                                acp_char_t    *aAttrValue,
                                                acp_sint32_t   aAttrValueLength)
{
    ulnDiscreteInt *sAllowedPair;
    ulvSLen         sValue;

    sAllowedPair = (ulnDiscreteInt *)gUlnConnAttrTable[aConnAttr].mCheck;

    ACI_TEST_RAISE(sAllowedPair == NULL, LABEL_NOT_IMPLEMENTED);

    while (sAllowedPair->mKey != NULL)
    {
        if ( (0 == acpCStrCaseCmp(sAllowedPair->mKey, aAttrValue, aAttrValueLength)) &&
             (sAllowedPair->mKey[aAttrValueLength] == '\0') )
        {
            break;
        }

        sAllowedPair++;
    }

    /*
     * ���� ������ Ű���� (ulnConnAttributre.cpp ������ ���̺��鿡�� ������ �� Ű����) ��
     * �ƴϸ�, ULN_CONN_ATTR_INVALID_VALUE �� sValue �� ���õ�.
     *
     * ������ ulnSetConnAttr() �Լ����� ���� üũ�ϸ鼭 �߻�.
     */

    sValue = sAllowedPair->mValue;
    /*
     * BUG-28980 [CodeSonar]strncpy Length Unreasonable
     * aAttrValueLength�� ���� ���� ������ ��� ���Ÿ� ����
     */
    ACI_TEST(ulnSetConnAttr(aFnContext,
                            aConnAttr,
                            (void *)(acp_slong_t)sValue,
                            ACI_SIZEOF(acp_slong_t)) != ACI_SUCCESS);

    /*
     * Note : ���� üũ�� ulnSetConnAttr() ���� �Ѵ�.
     */

    return ACI_SUCCESS;

    ACI_EXCEPTION(LABEL_NOT_IMPLEMENTED)
    {
        /* HYC00 : Optional feature not implemented */
        ulnError(aFnContext, ulERR_ABORT_OPTIONAL_FEATURE_NOT_IMPLEMENTED);
    }

    ACI_EXCEPTION_END;

    return ACI_FAILURE;
}






static ACI_RC ulnConnAttrProcessStringAttr(ulnFnContext  *aFnContext,
                                           ulnConnAttrID  aConnAttr,
                                           acp_char_t    *aAttrValue,
                                           acp_sint32_t   aLength)
{
    ulnReservedString *sReservedWordList;

    sReservedWordList = (ulnReservedString *)gUlnConnAttrTable[aConnAttr].mCheck;

    ACI_TEST_RAISE(sReservedWordList == NULL, LABEL_NOT_IMPLEMENTED);

    while (sReservedWordList->mKey != NULL)
    {
        if (aAttrValue != NULL)
        {
            if (0 == acpCStrCmp(aAttrValue, sReservedWordList->mKey, aLength) &&
                sReservedWordList->mKey[aLength] == '\0')
            {
                break;
            }
        }

        sReservedWordList++;
    }

    /* set up value   */
    if (sReservedWordList->mKey == NULL)
    {
        /*
         * ��ġ�Ǵ� ���ڿ��� ��� �Է¹��� ���ڿ��� �״�� �����ҷ��� �ϴµ�,
         * �Է¹��� ���ڿ��� NULL �̳� "" �̸�, NULL �� �����Ѵ�.
         */
        if (aAttrValue != NULL)
        {
            if (*aAttrValue == 0)
            {
                aAttrValue = NULL;
            }
        }
    }
    else
    {
        /*
         * ��ġ�Ǵ� ���ڿ��� �־ Ȯ���� �Ѵ�.
         */
        aAttrValue = (acp_char_t *)(sReservedWordList->mValue);
    }

    ACI_TEST(ulnSetConnAttr(aFnContext,
                            aConnAttr,
                            aAttrValue,
                            aLength) != ACI_SUCCESS);

    return ACI_SUCCESS;

    ACI_EXCEPTION(LABEL_NOT_IMPLEMENTED)
    {
        /* HYC00 : Optional feature not implemented */
        ulnError(aFnContext, ulERR_ABORT_OPTIONAL_FEATURE_NOT_IMPLEMENTED);
    }

    ACI_EXCEPTION_END;

    return ACI_FAILURE;
}

static ACI_RC ulnConnAttrProcessUpperCaseStringAttr(ulnFnContext  *aFnContext,
                                                    ulnConnAttrID  aConnAttr,
                                                    acp_char_t    *aAttrValue,
                                                    acp_sint32_t   aLength)
{
    ulnDbc          *sDbc;       //BUG-22684
    acp_char_t      *sTempAttrValue = NULL;
    acp_char_t      *sIndex;
    acp_char_t      *sFence;
    const mtlModule *sDefaultModule;// = mtlDefaultModule();

    //BUG-22684
    // Connection�� �̷������ ���� �Լ��� ȣ��� ��쿡�� sDbs�� NULL�̱� ������ �̶��� default Module�� ����ϵ��� �Ѵ�.
    ULN_FNCONTEXT_GET_DBC(aFnContext, sDbc);

    ACI_TEST( sDbc == NULL );           //BUG-28623 [CodeSonar]Null Pointer Dereference

    if ( sDbc->mClientCharsetLangModule == NULL )
    {
        sDefaultModule = mtlDefaultModule();
    }
    else
    {
        sDefaultModule = sDbc->mClientCharsetLangModule;
    }

    if (aLength > 0)
    {
        ACI_TEST_RAISE(acpMemAlloc((void**)&sTempAttrValue, aLength + 1) != ACP_RC_SUCCESS,
                       LABEL_NOT_ENOUGH_MEM);

        // PRJ-1678 : For multi-byte character set strings
        acpCStrCpy(sTempAttrValue, aLength + 1,
                   aAttrValue, aLength);

        sIndex = sTempAttrValue;
        sFence = sIndex + aLength;
        for(;sIndex < sFence;)
        {
            *sIndex = acpCharToUpper(*sIndex);
            ACI_TEST_RAISE(sDefaultModule->nextCharPtr((acp_uint8_t**)&sIndex,
                                                       (acp_uint8_t*)sFence)
                           != NC_VALID,
                           ERR_INVALID_CHARACTER );
        }
        *sFence = 0;
    }

    ACI_TEST(ulnConnAttrProcessStringAttr(aFnContext,
                                          aConnAttr,
                                          sTempAttrValue,
                                          aLength) != ACI_SUCCESS);

    if (sTempAttrValue != NULL)
    {
        acpMemFree(sTempAttrValue);
    }

    return ACI_SUCCESS;

    ACI_EXCEPTION(LABEL_NOT_ENOUGH_MEM)
    {
        ulnError(aFnContext,
                 ulERR_FATAL_MEMORY_ALLOC_ERROR,
                 "ulnConnAttrProcessUpperCaseStringAttr");
    }

    ACI_EXCEPTION(ERR_INVALID_CHARACTER)
    {
        ulnError(aFnContext,
                 ulERR_ABORT_INVALID_CHARACTER);
    }

    ACI_EXCEPTION_END;

    if (sTempAttrValue != NULL)
    {
        acpMemFree(sTempAttrValue);
    }

    return ACI_FAILURE;
}


ACI_RC ulnSetConnAttrById(ulnFnContext  *aFnContext,
                          ulnConnAttrID  aConnAttr,
                          acp_char_t    *aAttrValueString,
                          acp_sint32_t   aAttrValueStringLength)
{
    switch (gUlnConnAttrTable[aConnAttr].mAttrType)
    {
        case ULN_CONN_ATTR_TYPE_INT:
            ACI_TEST(ulnConnAttrProcessIntAttr(aFnContext,
                                               aConnAttr,
                                               aAttrValueString,
                                               aAttrValueStringLength) != ACI_SUCCESS);
            break;

        case ULN_CONN_ATTR_TYPE_DISCRETE_INT:
            ACI_TEST(ulnConnAttrProcessDiscreteIntAttr(aFnContext,
                                                       aConnAttr,
                                                       aAttrValueString,
                                                       aAttrValueStringLength) != ACI_SUCCESS);
            break;

        case ULN_CONN_ATTR_TYPE_STRING:
            ACI_TEST(ulnConnAttrProcessStringAttr(aFnContext,
                                                  aConnAttr,
                                                  aAttrValueString,
                                                  aAttrValueStringLength) != ACI_SUCCESS);
            break;

        case ULN_CONN_ATTR_TYPE_UPPERCASE_STRING:
            ACI_TEST(ulnConnAttrProcessUpperCaseStringAttr(aFnContext,
                                                           aConnAttr,
                                                           aAttrValueString,
                                                           aAttrValueStringLength) != ACI_SUCCESS);
            break;

        case ULN_CONN_ATTR_TYPE_CALLBACK:
        case ULN_CONN_ATTR_TYPE_POINTER:
            /*
             * Note : �������� attribute (callback ��) �� connection string �̳�
             *        profile string ���� ������ �� ����.
             *        ���� SQLSetConnectAttr() �θ� ������ �� �����Ƿ� �� �Լ��� Ÿ�� �ʴ´�.
             */
            ACI_RAISE(LABEL_MEM_MAN_ERR);
            break;

        default:
            ACE_ASSERT(0);
            break;
    }

    return ACI_SUCCESS;

    ACI_EXCEPTION(LABEL_MEM_MAN_ERR)
    {
        ulnError(aFnContext, ulERR_FATAL_MEMORY_MANAGEMENT_ERROR, "ulnSetConnAttrById");
    }

    ACI_EXCEPTION_END;

    return ACI_FAILURE;
}

acp_sint32_t ulnErrorConnStrPairIgnored( ulnFnContext     *aFnContext,
                                         const acp_char_t *aKey,
                                         acp_sint32_t      aKeyLen,
                                         const acp_char_t *aVal,
                                         acp_sint32_t      aValLen )
{
    #define ULN_CONNATTR_BUF_LEN 40

    acp_char_t sKey[ULN_CONNATTR_BUF_LEN + 1] = {'\0', };
    acp_char_t sVal[ULN_CONNATTR_BUF_LEN + 1] = {'\0', };

    acpCStrCpy(sKey, ACI_SIZEOF(sKey), aKey, aKeyLen);
    if (aKeyLen > ULN_CONNATTR_BUF_LEN)
    {
        /*
         * this code appends "..." in the end of string not the beginning.(BUG-39656)
         */
        acpCStrCpy( (sKey + ULN_CONNATTR_BUF_LEN + 1) - 5, 4, " ...", 4);
    }
    if (aVal != NULL)
    {
        acpCStrCpy(sVal, ACI_SIZEOF(sVal), aVal, aValLen);
        if (aValLen > ULN_CONNATTR_BUF_LEN)
        {
            acpCStrCpy( (sVal + ULN_CONNATTR_BUF_LEN + 1)- 5, 4, " ...", 4);
        }
    }
    else
    {
        acpCStrCpy(sVal, ACI_SIZEOF(sVal), "null", 4);
    }

    return ulnError(aFnContext, ulERR_IGNORE_CONNECTION_STR_IGNORED, sKey, sVal);

    #undef ULN_CONNATTR_BUF_LEN
}

acp_sint32_t ulnCallbackSetConnAttr( void                         *aContext,
                                     ulnConnStrParseCallbackEvent  aEvent,
                                     acp_sint32_t                  aPos,
                                     const acp_char_t             *aKey,
                                     acp_sint32_t                  aKeyLen,
                                     const acp_char_t             *aVal,
                                     acp_sint32_t                  aValLen,
                                     void                         *aFilter )
{
    ulnFnContext  *sFnContext = (ulnFnContext *) aContext;
    ulnConnAttrID  sFilterID;
    ulnConnAttrID  sConnAttrID;
    acp_sint32_t   sRC = 0;

    switch (aEvent)
    {
        case ULN_CONNSTR_PARSE_EVENT_GO:
            sFilterID = (ulnConnAttrID) aFilter;
            sConnAttrID = ulnGetConnAttrIDfromKEYWORD(aKey, aKeyLen);
            if ( (sFilterID == ULN_CONN_ATTR_MAX) || (sFilterID == sConnAttrID) )
            {
                if (sConnAttrID <  ULN_CONN_ATTR_MAX)
                {
                    ACI_TEST( ulnSetConnAttrById(sFnContext, sConnAttrID, (acp_char_t *)aVal, aValLen)
                              != ACI_SUCCESS );
                    sRC |= ULN_CONNSTR_PARSE_RC_SUCCESS;
                }
                else
                {
                    ACI_TEST( ulnErrorConnStrPairIgnored(sFnContext, aKey, aKeyLen, aVal, aValLen)
                              != ACI_SUCCESS );
                }

                if (sFilterID != ULN_CONN_ATTR_MAX)
                {
                    sRC |= ULN_CONNSTR_PARSE_RC_STOP;
                }
            }
            break;

        case ULN_CONNSTR_PARSE_EVENT_IGNORE:
            ACI_TEST( ulnErrorConnStrPairIgnored(sFnContext, aKey, aKeyLen, aVal, aValLen)
                      != ACI_SUCCESS );
            break;

        case ULN_CONNSTR_PARSE_EVENT_ERROR:
            /* ���� ���ڰ� ������ '?'�� */
            ACI_TEST( ulnError(sFnContext, ulERR_ABORT_INVALID_CONNECTION_STR_FORM,
                               aPos, (aKey == NULL) ? '?' : aKey[0])
                      != ACI_SUCCESS );
            break;

        default:
            ACE_ASSERT(0);
            break;
    }

    return sRC;

    ACI_EXCEPTION_END;

    return ULN_CONNSTR_PARSE_RC_ERROR;
}

#if defined(ALTIBASE_ODBC)

/* ------------------------------------------------
 *  ODBC
 * ----------------------------------------------*/

ACI_RC ulnSetConnAttrByProfileFunc(ulnFnContext  *aFnContext,
                                   acp_char_t    *aDSNString,
                                   acp_char_t    *aResourceName)
{
    ulnConnAttrID sConnAttr;

    acp_char_t    sBuffer[1024];
    acp_sint32_t  i;
    acp_sint32_t  sSize;

    /* Check The Resource for all aliases */
    for (i = 0; gUlnConnAttrMap_PROFILE[i].mKey != NULL; i++)
    {
        /*
         * unix-odbc
         *      setupSQLGetPrivateProfileString() ���� dlsym() �� ���� ���� �Լ��� ȣ��.
         *
         * sqlcli
         *      �� �Լ� ������ �ȵ�.
         *      �� �Լ� ��� �Ʒ����� �ƹ��͵� ���ϴ� ������ �����ϵ�.
         *
         * �ڼ��� ���� gSQLConnectModule �� tag �� ������.
         */
        sSize = gPrivateProfileFuncPtr(aDSNString,
                                       (acp_char_t *)(gUlnConnAttrMap_PROFILE[i].mKey),
                                       (acp_char_t *)"",
                                       sBuffer,
                                       ACI_SIZEOF(sBuffer),
                                       aResourceName);

        if (sSize > 0)
        {
            sConnAttr = gUlnConnAttrMap_PROFILE[i].mConnAttrID;

            /*
             * BUGBUG : null-terminating �ϴ� �κ��� �ּ�ó���Ǿ� �־���.
             *          SQLGetPrivateProfileString() �� null terminated string �� �شٴ� ������...
             *          ������ �о���� ����� ������...
             */

            ACI_TEST(ulnSetConnAttrById(aFnContext,
                                        sConnAttr,
                                        sBuffer,
                                        sSize) != ACI_SUCCESS);
        }
    }

    return ACI_SUCCESS;

    ACI_EXCEPTION_END;

    return ACI_FAILURE;
}
#else /* if defined(ALTIBASE_ODBC) */

/* ------------------------------------------------
 *  ODBCCLI
 * ----------------------------------------------*/

ACI_RC ulnSetConnAttrByProfileFunc(ulnFnContext *aFnContext,
                                   acp_char_t   *aDSNString,
                                   acp_char_t   *aResourceName)
{
    ACP_UNUSED(aFnContext);
    ACP_UNUSED(aDSNString);
    ACP_UNUSED(aResourceName);

    // nothing to do in cli

    return ACI_SUCCESS;
}
#endif /* if defined(ALTIBASE_ODBC) */

ACI_RC ulnSetConnAttrBySQLConstant(ulnFnContext *aFnContext,
                                   acp_sint32_t  aSQLConstant,
                                   void         *aValuePtr,
                                   acp_sint32_t  aLength)
{
    ulnConnAttrID sConnAttr;
    acp_sint32_t  sRealLength;

    /*
     * ulnConnAttrID ������
     */

    sConnAttr = ulnGetConnAttrIDfromSQL_ATTR_ID(aSQLConstant);

    ACI_TEST_RAISE(sConnAttr == ULN_CONN_ATTR_MAX, LABEL_INVALID_ATTRIBUTE);

    if (gUlnConnAttrTable[sConnAttr].mAttrType == ULN_CONN_ATTR_TYPE_STRING ||
        gUlnConnAttrTable[sConnAttr].mAttrType == ULN_CONN_ATTR_TYPE_UPPERCASE_STRING)
    {
        /*
         * Note : SQLSetConnectAttr() ������ string ���� �����ϴ� �Ӽ�����
         *        ����Ǵ� ����, reserved word üũ ���� �����ϱ� ���ؼ�
         *        ulnSetConnAttrById() �Լ��� ���ؼ� ulnSetConnAttr() �� ȣ���ؾ� �Ѵ�.
         *
         * sRealLength �� ���������� �ȵ�.
         */
        ACI_TEST_RAISE(aValuePtr == NULL, LABEL_INVALID_USE_OF_NULL);

        sRealLength = ulnAttrLength(aValuePtr, aLength);
        ACI_TEST_RAISE(sRealLength < 0, LABEL_INVALID_STRLEN);

        ACI_TEST(ulnSetConnAttrById(aFnContext,
                                    sConnAttr,
                                    (acp_char_t *)aValuePtr,
                                    sRealLength) != ACI_SUCCESS);
    }
    else
    {
        /*
         * string �� �ƴ� �Ϲ����� connection attribute �����ϱ�
         */
        ACI_TEST(ulnSetConnAttr(aFnContext,
                                sConnAttr,
                                aValuePtr,
                                aLength) != ACI_SUCCESS);
    }

    return ACI_SUCCESS;

    ACI_EXCEPTION(LABEL_INVALID_USE_OF_NULL);
    {
        /* HY009 : Invalid use of null pointer */
        ulnError(aFnContext, ulERR_ABORT_INVALID_USE_OF_NULL_POINTER);
    }

    ACI_EXCEPTION(LABEL_INVALID_STRLEN)
    {
        /* HY090 */
        ulnError(aFnContext, ulERR_ABORT_Invalid_StrLen_IndPtr_Error);
    }

    ACI_EXCEPTION(LABEL_INVALID_ATTRIBUTE)
    {
        /*
         * HY092 : Invalid attribute/option identifier
         */
        ulnError(aFnContext, ulERR_ABORT_INVALID_ATTR_OPTION, aSQLConstant);
    }

    ACI_EXCEPTION_END;

    return ACI_FAILURE;
}


/*
 * Note : 64 ��Ʈ odbc ����
 *
 * When the Attribute parameter has one of the following values,
 * a 64-bit value is passed in Value:
 *      SQL_ATTR_QUIET_MODE  --> ������� �����Ƿ� ����
 *
 * When the Option parameter has one of the following values,
 * a 64-bit value is passed in *Value:
 *      SQL_MAX_LENGTH
 *      SQL_MAX_ROWS
 *      SQL_ROWSET_SIZE
 *      SQL_KEYSET_SIZE
 *
 *      ��� ��� ���� �Ӽ����� ����� ���� ����.
 */

SQLRETURN ulnSetConnectAttr(ulnDbc *aDbc, acp_sint32_t aSQL_ATTR_ID, void *aValuePtr, acp_sint32_t aLength)
{
    ULN_FLAG(sNeedExit);
    ulnFnContext sFnContext;

    ulnDbc *sDbc;

    sDbc = ulnDbcSwitch(aDbc);

    ULN_INIT_FUNCTION_CONTEXT(sFnContext, ULN_FID_SETCONNECTATTR, sDbc, ULN_OBJ_TYPE_DBC);

    ACI_TEST(ulnEnter(&sFnContext, (void *)(&aSQL_ATTR_ID)) != ACI_SUCCESS);

    ULN_FLAG_UP(sNeedExit);

    /*
     * =====================
     * Function main
     * =====================
     */

    ACI_TEST(ulnSetConnAttrBySQLConstant(&sFnContext,
                                         aSQL_ATTR_ID,
                                         aValuePtr,
                                         aLength) != ACI_SUCCESS);

    /*
     * =====================
     * Function end
     * =====================
     */

    ULN_FLAG_DOWN(sNeedExit);

    ACI_TEST(ulnExit(&sFnContext) != ACI_SUCCESS);

    ULN_TRACE_LOG(&sFnContext, ULN_TRACELOG_MID, NULL, 0,
            "%-18s| [%2"ACI_INT32_FMT" ptr: %p]",
            "ulnSetConnectAttr", aSQL_ATTR_ID, aValuePtr);

    return ULN_FNCONTEXT_GET_RC(&sFnContext);

    ACI_EXCEPTION_END;

    ULN_IS_FLAG_UP(sNeedExit)
    {
        ulnExit(&sFnContext);
    }

    ULN_TRACE_LOG(&sFnContext, ULN_TRACELOG_LOW, NULL, 0,
            "%-18s| [%2"ACI_INT32_FMT" ptr: %p] fail",
            "ulnSetConnectAttr", aSQL_ATTR_ID, aValuePtr);

    return ULN_FNCONTEXT_GET_RC(&sFnContext);
}

/*
 * Meaningless callback
 */

ACI_RC ulnCallbackDBPropertySetResult(cmiProtocolContext *aProtocolContext,
                                      cmiProtocol        *aProtocol,
                                      void               *aServiceSession,
                                      void               *aUserContext)
{
    ACP_UNUSED(aProtocolContext);
    ACP_UNUSED(aProtocol);
    ACP_UNUSED(aServiceSession);
    ACP_UNUSED(aUserContext);

    /*
     * ���� ����.
     */
    return ACI_SUCCESS;
}

/**
 * BUG-36256 Improve property's communication
 *
 * ulnSetConnectAttrOff
 *
 * @aDbc        : Object
 * @aPropertyID : Property ID
 *
 * aPropertyID�� Off �Ѵ�.
 * ������ ���� �߰��Ǵ� ����-Ŭ���̾�Ʈ ������Ƽ�� ��� �߰��Ǿ�� �Ѵ�.
 * (ULN_PROPERTY_LOB_CACHE_THRESHOLD�� ����)
 *
 * tag�� ���� ������ ������ ���������� Ŭ���̾�Ʈ�� �����ϴ� ��� ��������
 * �������� �ʴ� ������Ƽ�� ������ ���� �Ѵ�. ���� �ݵ�� �����Ǿ�� �Ѵٸ�
 * INCOMPATIBLE_PROPERTY�� �߻����Ѿ� �Ѵ�. tag������ ȣȯ�� �ǵ��� ���ο�
 * ������Ƽ�� �ݵ�� Off �Ӽ��� ������ �Ѵ�.
 */
ACI_RC ulnSetConnectAttrOff(ulnFnContext  *aFnContext,
                            ulnDbc        *aDbc,
                            ulnPropertyId  aPropertyID)
{
    ulnConnAttrID sConnAttrID = ULN_CONN_ATTR_MAX;

    switch (aPropertyID)
    {
        case ULN_PROPERTY_LOB_CACHE_THRESHOLD:
            sConnAttrID = ULN_CONN_ATTR_LOB_CACHE_THRESHOLD;
            aDbc->mAttrLobCacheThreshold = ULN_LOB_CACHE_THRESHOLD_MIN;
            break;

            /* BUG-39817 */
        case ULN_PROPERTY_ISOLATION_LEVEL:
            sConnAttrID = ULN_CONN_ATTR_TXN_ISOLATION;
            aDbc->mAttrTxnIsolation = ULN_ISOLATION_LEVEL_DEFAULT;
            break;

        /* PROJ-2638 shard native linker */
        case ULN_PROPERTY_SHARD_LINKER_TYPE:
            sConnAttrID = ULN_CONN_ATTR_SHARD_LINKER_TYPE;
            aDbc->mShardDbcCxt.mShardLinkerType = 0;
            break;

        /* PROJ-2638 shard native linker */
        case ULN_PROPERTY_SHARD_NODE_NAME:
            sConnAttrID = ULN_CONN_ATTR_SHARD_NODE_NAME;
            aDbc->mShardDbcCxt.mShardTargetDataNodeName[0] = '\0';
            break;

        /*
         * ���ο� ������Ƽ �߰��� �Ʒ��� ���� �ڵ�� ������
         * ��������� �ʾƾ� �Ѵ�.
        case ULN_PRPOERTY_???_PROPERTY:
            sConnAttrID = ULN_CONN_ATTR_???;
            ACI_RAISE(LABEL_ABORT_INCOMPATIBLE_PROPERTY);
            break;
         */

        default:
            ACI_RAISE(LABEL_ABORT_INCOMPATIBLE_PROPERTY);
            break;
    }

    if (sConnAttrID != ULN_CONN_ATTR_MAX)
    {
        ACI_TEST_RAISE(ulnConnAttrArrAddId(&aDbc->mUnsupportedProperties,
                                           sConnAttrID),
                       LABEL_FATAL_NOT_ENOUGH_MEMORY);
    }
    else
    {
        /* nothing to do */
    }

    return ACI_SUCCESS;

    ACI_EXCEPTION(LABEL_ABORT_INCOMPATIBLE_PROPERTY)
    {
        ulnError(aFnContext,
                 ulERR_ABORT_INCOMPATIBLE_PROPERTY,
                 ulnGetConnAttrKEYWORDfromConnAttrId(sConnAttrID));
    }
    ACI_EXCEPTION(LABEL_FATAL_NOT_ENOUGH_MEMORY)
    {
        ulnError(aFnContext, ulERR_FATAL_MEMORY_ALLOC_ERROR,
                 "ulnSetConnectAttrOff");
    }
    ACI_EXCEPTION_END;

    return ACI_FAILURE;
}