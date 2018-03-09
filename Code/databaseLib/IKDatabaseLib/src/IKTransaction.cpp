/*
 * IKTransaction.cpp
 *
 *  Created on: 2014年5月5日
 *      Author: yqhe
 */

#include "IKTransaction.h"
#include "error.h"
#include "../../../Common/Exectime.h"

Transaction::Transaction(soci::session& sql)
	:_handled(false)
	,_sql(sql)
{
	Exectime etm(__FUNCTION__);
	_sql.begin();
}

Transaction::~Transaction()
{
	Exectime etm(__FUNCTION__);
	if (false == _handled)
	{
		try
		{
			Rollback();
		}

		catch (...)
		{
		}
	}
}


void Transaction::Commit()
{
	Exectime etm(__FUNCTION__);
	if (_handled)
	{
		TransactionErr();
	}

	_sql.commit();
	_handled = true;
}

void Transaction::Rollback()
{
	Exectime etm(__FUNCTION__);
	if (_handled)
	{
		TransactionErr();
	}

	_sql.rollback();
	_handled = false;

}

void Transaction::TransactionErr()
{
	Exectime etm(__FUNCTION__);
	throw soci::soci_error("The transaction object cannot be handled twice.");
}

