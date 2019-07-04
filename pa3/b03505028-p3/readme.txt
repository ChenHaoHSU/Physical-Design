1. 學號：B03505028
2. 姓名：徐晨皓
3. 使用之程式語言：< C++ >
4. 使用之編譯器：< GNU g++ >
5. 檔案解壓縮方式:< tar zxvf b03505028-p3.tar >
6. 各檔案說明：
    由於這次的作業是建在 NTUplace3 上, 以下僅列出跟這次作業相關之各檔案

    b03505028-p3/readme.txt： 程式說明純文字檔
	 b03505028-p3/report.doc： 作業報告(.doc)
    b03505028-p3/report.pdf： 作業報告(.pdf)    
    b03505028-p3/legalizer： 程式執行檔
    b03505028-p3/src/Legalizer/Legal.h: Source code
    b03505028-p3/src/Legalizer/Legal.cpp: Source code
    b03505028-p3/src/Legalizer/Cluster.h: Source code

           . . . . . .  
7. 編譯方式說明：        	
	 請在主程式的目錄下(b03505028-p3/src/)，先鍵入 "cmake ./" 的指令，
    然後再鍵入 make 指令, 即可完成編譯，( Makefile 原先下的 Optimize 指令為 -O3)
	 在主程式的目錄下(b03505028-p3/src/)會產生一名為 legalizer 的執行檔
	 如果要重新編譯，請先執行 make clean 再執行一次 make
    或者直接鍵入 make 即可
           . . . . . .  

8. 執行、使用方式說明：
   編譯完成後，在檔案目錄下會產生一個名為 legalizer 的執行檔
   執行檔的命令格式為：
      ./legalizer -aux [inputFile.aux]
   例如:
      ./legalizer -aux ibm01-cu85.aux
           . . . . . .   
      
9. 執行結果說明（說明執行結果的觀看方法，及解釋各項數據等）：
   主程式：
	當程式執行完畢且成功, 最後會出現以下訊息：
   ////// Result //////
   start check

   # row error: 0                  
   # site error: 0                  
   # overlap error: 0              // standard cells 最後重疊數
   Check success!!                 // 檢查成功訊息
   total displacement: 8.42155e+06 // 總位移量
   legalization success!           // legalization 成功訊息

   在執行檔的目錄下, 會產生一個 [inputFile].lg.plt, 可用 Gnuplot 觀看結果

   例如:
   若 input 檔為 ibm01-cu85.aux, 則會產生一個名為 ibm01-cu85.lg.plt
   可執行 
       gnuplot ibm01-cu85.lg.plt
   來觀看結果 







