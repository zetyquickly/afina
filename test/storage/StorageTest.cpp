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

TEST(StorageTest, MyTest) {
    MapBasedGlobalLockImpl storage;

    storage.Put("KEY1", "val1");
    storage.Put("KEY2", "val2");

    std::string value;
    EXPECT_TRUE(storage.Delete("KEY1")); 
    EXPECT_TRUE(storage.Delete("KEY2"));
    EXPECT_FALSE(storage.Get("KEY1", value));
    EXPECT_TRUE(storage.PutIfAbsent("KEY2", "SALAM"));
    EXPECT_FALSE(storage.PutIfAbsent("KEY2", "SALAM/2"));
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

    std::string str = "Key0000";
    size_t str_len = str.size();
    MapBasedGlobalLockImpl storage(2 * str_len * 100);

    std::stringstream ss;
    
    for(long i=0; i<100; ++i)
    {
        ss  << "Key" << std::setw(5) << std::setfill('0') << i;
        std::string key = ss.str();
        //std::cout << key <<std::endl;
        ss.str("");
        ss << "Val" << std::setw(5) << std::setfill('0') << i;
        std::string val = ss.str();
        ss.str("");
        storage.Put(key, val);
    }
    
    for(long i=99; i>=0; --i)
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
/*
TEST(StorageTest, MaxTest) {
    MapBasedGlobalLockImpl storage(1000);

    std::stringstream ss;

    for(long i=0; i<1100; ++i)
    {
        ss << "Key" << i;
        std::string key = ss.str();
        ss.str("");
        ss << "Val" << i;
        std::string val = ss.str();
        ss.str("");
        storage.Put(key, val);
    }
    
    for(long i=100; i<1100; ++i)
    {
        ss << "Key" << i;
        std::string key = ss.str();
        ss.str("");
        ss << "Val" << i;
        std::string val = ss.str();
        ss.str("");
        
        std::string res;
        storage.Get(key, res);

        EXPECT_TRUE(val == res);
    }
    
    for(long i=0; i<100; ++i)
    {
        ss << "Key" << i;
        std::string key = ss.str();
        ss.str("");
        
        std::string res;
        EXPECT_FALSE(storage.Get(key, res));
    }
}
*/