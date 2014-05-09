/**
 * Copyright (c) 2011 10gen Inc.
 * Copyright (C) 2013 Tokutek Inc.
 *
 * This program is free software: you can redistribute it and/or  modify
 * it under the terms of the GNU Affero General Public License, version 3,
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "mongo/pch.h"

#include "mongo/db/pipeline/expression_context.h"

#include "mongo/db/interrupt_status.h"

namespace mongo {

    ExpressionContext::~ExpressionContext() {
    }

    inline ExpressionContext::ExpressionContext(InterruptStatus *pS):
        doingMerge(false),
        inShard(false),
        inRouter(false),
        intCheckCounter(1),
        pStatus(pS) {
    }

    void ExpressionContext::checkForInterrupt() {
        pStatus->checkForInterrupt();
    }

    ExpressionContext* ExpressionContext::clone() {
        ExpressionContext* newContext = create(pStatus);
        newContext->setDoingMerge(getDoingMerge());
        newContext->setInShard(getInShard());
        newContext->setInRouter(getInRouter());
        return newContext;
    }

    ExpressionContext *ExpressionContext::create(InterruptStatus *pStatus) {
        return new ExpressionContext(pStatus);
    }

}
