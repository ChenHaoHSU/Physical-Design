1. 學號：B03505028
2. 姓名：徐晨皓
3. 使用之程式語言：< C++ >
4. 使用之編譯器：< GNU g++ >
5. 檔案解壓縮方式:< tar zxvf b03505028-p4.tar >
6. 各檔案說明：
   b03505028-p4/readme.txt： 程式說明純文字檔
   b03505028-p4/report.doc： 作業報告(.doc)
   b03505028-p4/report.pdf： 作業報告(.pdf)
   b03505028-p4/router： 執行檔
   b03505028-p4/src： 所有 Source code 的資料夾
	
           . . . . . .  
7. 編譯方式說明：        	
	請在主程式的目錄下(b03505028-p4)，鍵入make指令，即可完成編譯，
	( Makefile 原先下的 Optimize 指令為 -O3)
	在主程式的目錄下會產生一名為 router 的執行檔
	如果要重新編譯，請先執行 make clean 再執行一次 make
   或者直接鍵入 make 即可
           . . . . . .  
8. 執行、使用方式說明：
   編譯完成後，在檔案目錄下會產生一個名為 Floorplan.exe 的執行檔
   執行檔的命令格式為：
      ./router <input_file> <output_file>

   例如： 
      ./router case1.in case1.out
   則在同一個目錄下會產生一個 output file 名為 case1.out
   記錄 case1.in 的 Steiner tree 結果

   [補充] GNUPLOT OUTPUT FORMATS :
   若命令格式輸入為
      ./router <input_file> <output_file> -plt
   則在同一目錄下除了會產生這次作業的 output file
   同時也會產生兩個 Gnuplot formats 的檔案
   一個檔案為 input_file.graph.plt，
   顯示 spanning graph, minimum spanning tree, rectilinear Steiner tree
   另一個則為 input_file.tree.plt，
   只有顯示 rectilinear Steiner tree

   執行命令格式為:
      gnuplot *.plt
   即可觀看結果
   
           . . . . . .         
9. 執行結果說明（說明執行結果的觀看方法，及解釋各項數據等）：
   主程式：
   當程式執行完畢, 最後會出現以下訊息：
   
   >> Running Time: xxx seconds. // 程式總執行時間

   結果的格式為:
      NumRoutedPins = <number> // 所連到的 Pins 數量
      WireLength = <number>  // Steiner tree 總長
      H-line (x1,y) (x2,y)  // 水平線以及其兩端點座標
      V-line (x,y1) (x,y2)  // 鉛直線以及其兩端點座標
      ....

   






