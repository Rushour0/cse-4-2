import java.io.File;
import java.io.FileNotFoundException;
import java.util.*;

public class Main {
    static Set<String> getKeyWords() {
        Set<String> keywords = new HashSet<String>();
        try {
            File file = new File("keywords.txt");
            Scanner reader = new Scanner(file);
            while (reader.hasNextLine()) {
                String data = reader.nextLine().strip();
                if (data.split(" ").length > 1) {
                    throw new Exception("Keyword contains more than 1 word - " + data);
                }
                keywords.add(data);
            }
            reader.close();
        } catch (FileNotFoundException e) {
            System.out.println("An error occurred.");
            e.printStackTrace();
        } catch (Exception e) {
            System.out.println(e);
        }

        return keywords;

    }

    public static void main(String[] args) {
        // Overall commands that will be read from the file
        ArrayList<ArrayList<String>> commands = new ArrayList<ArrayList<String>>();

        Set<String> keywords = getKeyWords();
        Map<String, Integer> labels = new HashMap<String, Integer>();

        ArrayList<String> temp = new ArrayList<String>();
        try {
            File file = new File("assignment1.txt");
            Scanner reader = new Scanner(file);

            System.out.println("No.\tLabel\tCommand\tParams");
            Integer counter = 0;
            while (reader.hasNextLine()) {
                String data = reader.nextLine();
                temp = new ArrayList<String>(Arrays.asList(data.split(" ")));

                Integer currentSize = commands.size();

                // If the first word in the command is not present in the keywords
                if (keywords.contains(temp.get(0))) {
                    temp.add(0, "");
                } else {
                    if (!temp.get(1).trim().equals("START") && !temp.get(1).trim().equals("END"))
                        labels.put(temp.get(0), counter);
                }

                if (temp.get(1).trim().equals("START")) {
                    counter = Integer.parseInt(temp.get(2));
                }
                if (!temp.get(1).trim().equals("START") && !temp.get(1).trim().equals("END")) {
                    temp.add(0, counter.toString());
                    counter++;
                }
                commands.add(temp);
            }
            for (int i = 0; i < commands.size(); i++) {
                ArrayList<String> onecommand = commands.get(i);
                for (int j = 0; j < onecommand.size(); j++)
                    System.out.print(onecommand.get(j) + (j == onecommand.size() - 1 ? "" : "\t"));
                System.out.println();
            }

            for (Map.Entry<String, Integer> entry : labels.entrySet()) {
                System.out.println("Label: " + entry.getKey() + " Location: " + entry.getValue().toString());
            }

            reader.close();
        } catch (FileNotFoundException e) {
            System.out.println("An error occurred.");
            e.printStackTrace();
        }
    }
}