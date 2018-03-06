#include "gtest/gtest.h"
#include <iostream>
#include <set>
#include <vector>
#include <iomanip>

#include <storage/MapBasedGlobalLockImpl.h>
#include <afina/execute/Get.h>
#include <afina/execute/Set.h>
#include <afina/execute/Add.h>
#include <afina/execute/Append.h>
#include <afina/execute/Delete.h>

using namespace Afina::Backend;
using namespace Afina::Execute;
using namespace std;

TEST (StorageTest, Deletion) {
    MapBasedGlobalLockImpl storage;

    storage.Put("KEY1", "val1");
    storage.Put("KEY2", "val2");
    storage.Delete("KEY1"); 
    storage.Delete("KEY2");
    storage.Put("KEY3", "val3");
    // add output to Entry destructor
}


TEST(StorageTest, MyTest1) {
    MapBasedGlobalLockImpl storage;

    storage.Put("KEY1", "val1");
    storage.Put("KEY2", "val2");

    std::string value;
    EXPECT_TRUE(storage.Delete("KEY1")); 
    EXPECT_TRUE(storage.Delete("KEY2"));
    EXPECT_FALSE(storage.Get("KEY1", value));
    std::string key2 = "KEY2";
    EXPECT_TRUE(storage.PutIfAbsent(key2, "SALAM"));
    EXPECT_TRUE(storage.Put(key2, "SALAM/2"));
    EXPECT_FALSE(storage.Get("KEY1", value));
    EXPECT_TRUE(storage.Get("KEY2", value));
    EXPECT_TRUE(storage.Delete("KEY2"));
    std::stringstream ss;
    ss.str("KEY3");
    EXPECT_TRUE(storage.PutIfAbsent(ss.str(), "SALAM"));
    ss.str("");
    EXPECT_TRUE(storage.Get("KEY3", value));
    std::cout << value << std::endl;

}

TEST(StorageTest, MyTest2) {
    MapBasedGlobalLockImpl storage;

    std::string key1 = "key";
    std::string value1 = "value";

    EXPECT_TRUE(storage.PutIfAbsent(key1, value1));
}

TEST(StorageTest, PutGet) {
    MapBasedGlobalLockImpl storage;

    storage.Put("KEY1", "val1");
    storage.Put("KEY2", "val2");

    std::string value;
    EXPECT_TRUE(storage.Get("KEY1", value));
    EXPECT_TRUE(value == "val1");

    EXPECT_TRUE(storage.Get("KEY2", value));
    EXPECT_TRUE(value == "val2");
}

TEST(StorageTest, PutOverwrite) {
    MapBasedGlobalLockImpl storage;

    storage.Put("KEY1", "val1");
    storage.Put("KEY1", "val2");

    std::string value;
    EXPECT_TRUE(storage.Get("KEY1", value));
    EXPECT_TRUE(value == "val2");
}

TEST(StorageTest, PutIfAbsent) {
    MapBasedGlobalLockImpl storage;

    storage.Put("KEY1", "val1");
    storage.PutIfAbsent("KEY1", "val2");

    std::string value;
    EXPECT_TRUE(storage.Get("KEY1", value));
    EXPECT_TRUE(value == "val1");
}

TEST(StorageTest, BigTest) {

    const int N = 100000;
    std::string str = "Key0000";
    size_t str_len = 8;
    //cout << str_len << endl;
    MapBasedGlobalLockImpl storage(2 * str_len * N);

    std::stringstream ss;
    
    for(long i=0; i<N; ++i)
    {
        ss  << "Key" << std::setw(5) << std::setfill('0') << i;
        std::string key = ss.str();
        //std::cout << key <<std::endl;
        ss.str("");
        ss << "Val" << std::setw(5) << std::setfill('0') << i;
        std::string val = ss.str();
        ss.str("");
        //cout << val.size() << endl;
        storage.Put(key, val);
    }
    
    for(long i=N-1; i>=0; --i)
    {
        ss  << "Key" << std::setw(5) << std::setfill('0') << i;
        std::string key = ss.str();
        //std::cout << key <<std::endl;
        ss.str("");
        ss << "Val" << std::setw(5) << std::setfill('0') << i;
        std::string val = ss.str();
        ss.str("");
        
        std::string res;
        storage.Get(key, res);

        EXPECT_TRUE(val == res);
    }

}

TEST(StorageTest, MaxTest) {
    const int N = 1000;
    MapBasedGlobalLockImpl storage(2*N*8);

    std::stringstream ss;

    for(long i=0; i<N + 100; ++i)
    {
        ss << "Key" << std::setw(5) << std::setfill('0') << i;
        std::string key = ss.str();
        ss.str("");
        ss << "Val" << std::setw(5) << std::setfill('0') << i;
        std::string val = ss.str();
        ss.str("");
        storage.Put(key, val);
    }
    
    for(long i=100; i< N + 100; ++i)
    {
        ss << "Key" << std::setw(5) << std::setfill('0') << i;
        std::string key = ss.str();
        ss.str("");
        ss << "Val" << std::setw(5) << std::setfill('0') << i;
        std::string val = ss.str();
        ss.str("");
        
        std::string res;
        storage.Get(key, res);

        EXPECT_TRUE(val == res);
    }
    
    for(long i=0; i<100; ++i)
    {
        ss << "Key" << std::setw(5) << std::setfill('0') << i;
        std::string key = ss.str();
        ss.str("");
        
        std::string res;
        EXPECT_FALSE(storage.Get(key, res));
    }
}
