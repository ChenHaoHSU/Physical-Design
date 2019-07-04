1. 學號：B03505028
2. 姓名：徐晨皓
3. 使用之程式語言：< C++ >
4. 使用之編譯器：< GNU g++ >
5. 檔案解壓縮方式:tar zxvf b03505028-p1.tar
6. 各檔案說明：
	b03505028-p1/Makefile： Makefile
	b03505028-p1/main.cpp： Source code
	b03505028-p1/Net.h： Source code
	b03505028-p1/Net.cpp： Source code
	b03505028-p1/myHashSet.h： Source code
   b03505028-p1/fm：執行檔
   b03505028-p1/readme.txt： 程式說明純文字檔
	b03505028-p1/report.doc： 作業報告(.doc)
   b03505028-p1/report.doc： 作業報告(.pdf)
           . . . . . .  
7. 編譯方式說明：        	
	 請在主程式的目錄下(b03505028-p1)，鍵入make指令，即可完成編譯，
	 ( Makefile 原先下的 Optimize 指令為 -O3)
	 在主程式的目錄下會產生一名為 fm 的執行檔
	 如果要重新編譯，請先執行 make clean 再執行一次 make
    或者直接鍵入 make 即可
           . . . . . .  
8. 執行、使用方式說明：
   編譯完成後，在檔案目錄下會產生一個名為 fm 的執行檔
   執行檔的命令格式為：
   ./fm [input_file_name] [output_file_name]

   例如：
   ./fm input_1.dat 1.out
   則在同一個目錄下會產生一個 output file 名為 1.out
   記錄 input_1.dat 的 Partition 結果
           . . . . . .         
9. 執行結果說明（說明執行結果的觀看方法，及解釋各項數據等）：
   主程式：
	當程式執行完畢, 最後會出現以下訊息：
   > The result has been written to "OUTPUT_FILE_NAME". //成功將結果寫入 output file
   >> Running time: xxx seconds.                        //程式總執行時間






