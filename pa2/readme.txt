1. 學號：B03505028
2. 姓名：徐晨皓
3. 使用之程式語言：< C++ >
4. 使用之編譯器：< GNU g++ >
5. 檔案解壓縮方式:< tar zxvf b03505028-p2.tar >
6. 各檔案說明：
   b03505028-p2/readme.txt： 程式說明純文字檔
	b03505028-p2/report.doc： 作業報告(.doc)
   b03505028-p2/report.pdf： 作業報告(.pdf)
   b03505028-p2/Floorplan.exe： 執行檔
	b03505028-p2/Makefile： Makefile
	b03505028-p2/main.cpp：        Source code
	b03505028-p2/mydefine.h：      Source code
	b03505028-p2/floorplanner.h：  Source code
	b03505028-p2/perser.cpp：      Source code
	b03505028-p2/floorplan.cpp：   Source code
	b03505028-p2/result.cpp：      Source code
	b03505028-p2/utility.cpp：     Source code
	b03505028-p2/block.h：         Source code
	b03505028-p2/block.cpp：       Source code
	b03505028-p2/terminal.h：      Source code
	b03505028-p2/net.h：           Source code
	b03505028-p2/btree.h：         Source code
	b03505028-p2/btree.cpp：       Source code
	b03505028-p2/contourline.h：   Source code
	b03505028-p2/contourline.cpp： Source code
	b03505028-p2/myHashSet.h：     Source code
	b03505028-p2/gnuplot_i.h：     Source code

           . . . . . .  
7. 編譯方式說明：        	
	 請在主程式的目錄下(b03505028-p2)，鍵入make指令，即可完成編譯，
	 ( Makefile 原先下的 Optimize 指令為 -O3)
	 在主程式的目錄下會產生一名為 Floorplan.exe 的執行檔
	 如果要重新編譯，請先執行 make clean 再執行一次 make
    或者直接鍵入 make 即可
           . . . . . .  
8. 執行、使用方式說明：
   編譯完成後，在檔案目錄下會產生一個名為 Floorplan.exe 的執行檔
   執行檔的命令格式為：
   ./Floorplan.exe [α value] [input.block name] [input.net name] [output file name]

   例如：
   ./Floorplan.exe 0.5 case1.block case1.nets case1.out
   則在同一個目錄下會產生一個 output file 名為 case1.out
   記錄 case1.block case1.nets 的 Floorplan 結果

   [補充] GUI 功能:
   這個程式有 GUI 的功能, 只要有安裝 gnuplot 即可執行   
      $ sudo apt-get install gnuplot
   當安裝完畢後, 開啟 mydefine.h, 
   將 "// #define GNU_PLOT" 的註解刪除, 
   再重新鍵入make指令，即可完成編譯
   
           . . . . . .         
9. 執行結果說明（說明執行結果的觀看方法，及解釋各項數據等）：
   主程式：
	當程式執行完畢, 最後會出現以下訊息：
   > Floorplan result:            
     - Chip Boundary = xxxx  // Floorplan 的邊界
     - Chip Area     = xxxx  // Floorplan 的面積
     - Wire Length   = xxxx  // Floorplan nets 的繞線總長
     - Final Cost    = xxxx  // Floorplan 最後的 cost: αA + (1-α)W
   > The result has been written to "OUTPUT_FILE_NAME". //成功將結果寫入 output file
   >> Running time: xxx seconds.                        //程式總執行時間






