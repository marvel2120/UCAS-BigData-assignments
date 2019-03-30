import java.io.*;
import java.util.*;

public class Hw1Grp5 {
    public static boolean checkSame(ArrayList<String> s1, ArrayList<String> s2, ArrayList<Integer> nums){
        for(int i=0;i<nums.size();i++){
            if(!s1.get(nums.get(i)).equals(s2.get(nums.get(i)))) {
                return false;
            }
        }
        return true;
    }
    public static void main(String[] args){
        /*
        Get parameters from command line
        Give the vairables with corresponding values
         */
        String fileName = args[0].substring(2,args[0].length());
        int optCol = Integer.parseInt(args[1].substring(8,9));
        String opt = args[1].substring(10,12);
        float optValue = Float.parseFloat(args[1].substring(13,args[1].length()));
        ArrayList<Integer> disCol = new ArrayList<Integer>();
        for(int i=0; i<args[2].length(); i++){
            if(args[2].charAt(i)=='R'){
                disCol.add(Integer.parseInt(String.valueOf(args[2].charAt(i+1))));
            }
        }

        /*
        Use a list of list to store all the table data,
        one inner list maps a row in the table
         */

        List<ArrayList<String>> text = new ArrayList<ArrayList<String>>();
        /*
        Read data operation
         */
        try{
            FileReader fr = new FileReader(fileName);
            BufferedReader br = new BufferedReader((fr));
            String str;
            while ((str=br.readLine())!=null){
                String[] aa = str.split("\\|");
                ArrayList<String> temp = new ArrayList<String>();
                for(int i=0;i<aa.length;i++) {
                    temp.add(aa[i]);
                }
                text.add(temp);
            }
            br.close();
            fr.close();
        }catch (IOException e){
            e.printStackTrace();
        }

        /*
        Select which operator to use
         */
        if(opt.equals("gt")){
            for(int i=0;i<text.size();i++){
                if(Float.parseFloat(text.get(i).get(optCol)) <= optValue){
                    text.remove(text.get(i));
                    i--;
                }
            }
        }else if(opt.equals("ge")){
            for(int i=0;i<text.size();i++){
                if(Float.parseFloat(text.get(i).get(optCol)) < optValue){
                    text.remove(text.get(i));
                    i--;
                }
            }
        }else if(opt.equals("eq")){
            for(int i=0;i<text.size();i++){
                if(Float.parseFloat(text.get(i).get(optCol)) != optValue){
                    text.remove(text.get(i));
                    i--;
                }
            }
        }else if(opt.equals("ne")){
            for(int i=0;i<text.size();i++){
                if(Float.parseFloat(text.get(i).get(optCol)) == optValue){
                    text.remove(text.get(i));
                    i--;
                }
            }
        }else if(opt.equals("le")){
            for(int i=0;i<text.size();i++){
                if(Float.parseFloat(text.get(i).get(optCol)) > optValue){
                    text.remove(text.get(i));
                    i--;
                }
            }
        }else if(opt.equals("lt")){
            for(int i=0;i<text.size();i++){
                if(Float.parseFloat(text.get(i).get(optCol)) >= optValue){
                    text.remove(text.get(i));
                    i--;
                }
            }
        }


        /*
        Override comparison function
         */
//        Collections.sort(text, new Comparator<ArrayList<String>>() {
//            @Override
//            public int compare(ArrayList<String> o1, ArrayList<String> o2) {
//                for(int i=0;i<disCol.size();i++) {
//                    int check = 0; //默认是数字
//                    try {
//                        float num=Float.valueOf(o1.get(disCol.get(i)));//把字符串强制转换为数字
//                    } catch (Exception e) {
//                        check = 1;//说明是字符
//                    }
//                    if(check==0){
//                        int result = Float.valueOf(o1.get(disCol.get(i))).compareTo(Float.valueOf(o2.get(disCol.get(i))));
//                        if (result != 0) {
//                            return result;
//                        }
//                    }else {
//                        int result = o1.get(disCol.get(i)).compareTo(o2.get(disCol.get(i)));
//                        if (result != 0) {
//                            return result;
//                        }
//                    }
//                }
//                return 0;
//            }
//        });


//        大小写顺序
//        14和140前后
        Collections.sort(text, new Comparator<ArrayList<String>>() {
            @Override
            public int compare(ArrayList<String> o1, ArrayList<String> o2) {
                for(int i=0;i<disCol.size();i++) {
                    int result = o1.get(disCol.get(i)).compareTo(o2.get(disCol.get(i)));
                    if (result != 0) {
                        return result;
                    }
                }
                return 0;
            }
        });

        /*
        Remove same entries
         */

        for(int i=0;i<text.size()-1;i++){
            if(checkSame(text.get(i),text.get(i+1),disCol)){
                text.remove(text.get(i+1));
                i--;
            }
        }
        for(int i=0;i<text.size();i++){
            System.out.println(text.get(i));
        }

    }
}
