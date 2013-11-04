// counttests.cpp : count.{h,cpp} unit tests.

/**
 *    Copyright (C) 2008 10gen Inc.
 *    Copyright (C) 2013 Tokutek Inc.
 *
 *    This program is free software: you can redistribute it and/or  modify
 *    it under the terms of the GNU Affero General Public License, version 3,
 *    as published by the Free Software Foundation.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Affero General Public License for more details.
 *
 *    You should have received a copy of the GNU Affero General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <boost/thread/thread.hpp>

#include "mongo/db/cursor.h"
#include "mongo/db/db.h"
#include "mongo/db/json.h"
#include "mongo/db/ops/count.h"
#include "mongo/db/structure/collection.h"

#include "mongo/dbtests/dbtests.h"

namespace CountTests {

    class Base {
        Lock::DBWrite lk;
        Client::Context _context;
        Database* _database;
        Collection* _collection;
    public:
        Base() : lk(ns()), _context( ns() ) {
            _database = _context.db();
            _collection = _database->getCollection( ns() );
            if ( _collection ) {
                _database->dropCollection( ns() );
            }
            _collection = _database->createCollection( ns(), false, NULL, true );

            addIndex( fromjson( "{\"a\":1}" ) );
        }
        ~Base() {

            try {
                uassertStatusOK( _database->dropCollection( ns() ) );
            }
            catch ( ... ) {
                FAIL( "Exception while cleaning up collection" );
            }
        }
    
        static const char *ns() {
            return "unittests.counttests";
        }
        void addIndex( const BSONObj &key ) {
            BSONObjBuilder b;
            b.append( "name", key.firstElementFieldName() );
            b.append( "ns", ns() );
            b.append( "key", key );
            BSONObj o = b.done();
            Status s = _collection->getIndexCatalog()->createIndex( o, false );
            uassertStatusOK( s );
        }
        void insert( const char *s ) {
            insert( fromjson( s ) );
        }
        void insert( const BSONObj &o ) {
            _collection->insertDocument( o, false );
        }
        static BSONObj countCommand( const BSONObj &query ) {
            return BSON( "query" << query );
        }
    };
    
    class Basic : public Base {
    public:
        void run() {
            insert( "{\"a\":\"b\"}" );
            BSONObj cmd = fromjson( "{\"query\":{}}" );
            string err;
            int errCode;
            ASSERT_EQUALS( 1, runCount( ns(), cmd, err, errCode ) );
        }
    };
    
    class Query : public Base {
    public:
        void run() {
            insert( "{\"a\":\"b\"}" );
            insert( "{\"a\":\"b\",\"x\":\"y\"}" );
            insert( "{\"a\":\"c\"}" );
            BSONObj cmd = fromjson( "{\"query\":{\"a\":\"b\"}}" );
            string err;
            int errCode;
            ASSERT_EQUALS( 2, runCount( ns(), cmd, err, errCode ) );
        }
    };
    
    class Fields : public Base {
    public:
        void run() {
            insert( "{\"a\":\"b\"}" );
            insert( "{\"c\":\"d\"}" );
            BSONObj cmd = fromjson( "{\"query\":{},\"fields\":{\"a\":1}}" );
            string err;
            int errCode;
            ASSERT_EQUALS( 2, runCount( ns(), cmd, err, errCode ) );
        }
    };
    
    class QueryFields : public Base {
    public:
        void run() {
            insert( "{\"a\":\"b\"}" );
            insert( "{\"a\":\"c\"}" );
            insert( "{\"d\":\"e\"}" );
            BSONObj cmd = fromjson( "{\"query\":{\"a\":\"b\"},\"fields\":{\"a\":1}}" );
            string err;
            int errCode;
            ASSERT_EQUALS( 1, runCount( ns(), cmd, err, errCode ) );
        }
    };
    
    class IndexedRegex : public Base {
    public:
        void run() {
            insert( "{\"a\":\"b\"}" );
            insert( "{\"a\":\"c\"}" );
            BSONObj cmd = fromjson( "{\"query\":{\"a\":/^b/}}" );
            string err;
            int errCode;
            ASSERT_EQUALS( 1, runCount( ns(), cmd, err, errCode ) );
        }
    };

    /** Set a value or await an expected value. */
    class PendingValue {
    public:
        PendingValue( int initialValue ) :
            _value( initialValue ),
            _mutex( "CountTests::PendingValue::_mutex" ) {
        }
        void set( int newValue ) {
            scoped_lock lk( _mutex );
            _value = newValue;
            _condition.notify_all();
        }
        void await( int expectedValue ) const {
            scoped_lock lk( _mutex );
            while( _value != expectedValue ) {
                _condition.wait( lk.boost() );
            }
        }
    private:
        int _value;
        mutable mongo::mutex _mutex;
        mutable boost::condition _condition;
    };

    /** A writer client will be registered for the lifetime of an object of this class. */
    class WriterClientScope {
    public:
        WriterClientScope() :
            _state( Initial ),
            _dummyWriter( boost::bind( &WriterClientScope::runDummyWriter, this ) ) {
            _state.await( Ready );
        }
        ~WriterClientScope() {
            // Terminate the writer thread even on exception.
            _state.set( Finished );
            DESTRUCTOR_GUARD( _dummyWriter.join() );
        }
    private:
        enum State {
            Initial,
            Ready,
            Finished
        };
        void runDummyWriter() {
            Client::initThread( "dummy writer" );
            scoped_ptr<Acquiring> a( new Acquiring( 0 , cc().lockState() ) );
            _state.set( Ready );
            _state.await( Finished );
            a.reset(0);
            cc().shutdown();
        }
        PendingValue _state;
        boost::thread _dummyWriter;
    };
    
    class All : public Suite {
    public:
        All() : Suite( "count" ) {
        }
        
        void setupTests() {
            add<Basic>();
            add<Query>();
            add<Fields>();
            add<QueryFields>();
            add<IndexedRegex>();
        }
    } myall;
    
} // namespace CountTests
