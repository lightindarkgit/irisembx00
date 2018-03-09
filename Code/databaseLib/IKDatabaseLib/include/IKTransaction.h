/*
 * IKTransaction.h
 *
 *  Created on: 2014年5月5日
 *      Author: yqhe
 */

#pragma once

#include "session.h"
#include "IKDatabaseType.h"

class IKDBLIB_API Transaction
{
public:
	explicit Transaction(soci::session& sql);

	~Transaction();

	void Commit();
	void Rollback();

private:
	bool _handled;
	soci::session& _sql;

private:
	// Disable copying
	Transaction(Transaction const& other) = delete;
	Transaction& operator=(Transaction const& other) = delete;

	void TransactionErr();

};

