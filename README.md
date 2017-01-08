# TsStreamParser

TS 流全称是 Transport Stream，TS 流文件是一种 MPEG2-TS 格式的音视频复合流文件，其特点是可以从视频流的任一片段开始独立解码。本项目为实习期基于 C 语言实现的 TS 流解析，意在通过简单的解析音视频来理解复杂的 TS 流编码格式，该项目只实现了 TS 流部分表的解析，其测试案例由于没时间编写 GUI，故使用了 Windows 下的 PrintTerminal 进行模拟交互（入口在 PrintTerminal.c），如需在 unix 下运行请修改 PrintTerminal 的实现即可。

相关 TS 流格式分析工具及编码文档参见官方国际标准文档即可。

# License

MIT License

Copyright (c) 2017 yanbo

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
