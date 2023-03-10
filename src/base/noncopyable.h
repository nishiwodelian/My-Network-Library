#pragma once

/*
在派生类能够正常定义对象，保证类中数据安全的前提下，禁止派生类编译器自动生成拷贝构造函数和拷贝赋值函数

《C++ Primer 第5版》15.7.2 合成拷贝控制与继承" 这一章节里有如下内容：
如果基类中的默认构造函数、拷贝构造函数、拷贝赋值运算符或析构函数是被删除的函数或者不可访问，则派生类中对应的成员将是被删除的，原因是编译器不能使用基类成员来执行派生类对象基类部分的构造、赋值或销毁操作。
*/ 


class noncopyable{
    protected:
     noncopyable() = default;
     ~noncopyable() = default;

    private:
     noncopyable(const noncopyable&) = delete;   //这是个noncopyable类类型的引用，函数声明可以省略形参名字的
     const noncopyable& operator=(const noncopyable&) = delete;
};