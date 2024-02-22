import java.io.File;
import java.io.FileNotFoundException;
import java.util.*;

enum StatementClass {
    IS, DL, AD
};

class OpTableValue {
    StatementClass statementClass;
    Integer machineCode;

    OpTableValue(String statementClass, Integer machineCode) {
        this.statementClass = statementClass.equals("IS") ? StatementClass.IS
                : statementClass.equals("DL") ? StatementClass.DL : StatementClass.AD;
        this.machineCode = machineCode;
    }
}

public class Main {
    static Map<String, OpTableValue> getOpTable() {
        Map<String, OpTableValue> opTable = new HashMap<String, OpTableValue>();
        try {
            File file = new File("optable.txt");
            Scanner reader = new Scanner(file);
            while (reader.hasNextLine()) {
                String data = reader.nextLine();
                String[] temp = data.split(" ");
                opTable.put(temp[0], new OpTableValue(temp[1], Integer.parseInt(temp[2])));
            }
            reader.close();
        } catch (FileNotFoundException e) {
            System.out.println("An error occurred.");
            e.printStackTrace();
        }
        return opTable;
    }

    static Map<String, Integer> getSymbolTable() {
        Map<String, Integer> symbolTable = new HashMap<String, Integer>();
        try {
            File file = new File("symboltable.txt");
            Scanner reader = new Scanner(file);
            while (reader.hasNextLine()) {
                String data = reader.nextLine();
                String[] temp = data.split(" ");
                symbolTable.put(temp[0], Integer.parseInt(temp[1]));
            }
            reader.close();
        } catch (FileNotFoundException e) {
            System.out.println("An error occurred.");
            e.printStackTrace();
        }
        return symbolTable;
    }
    static void printTable(ArrayList<ArrayList<String>> commands) {
        // Print out the table
        System.out.println("Address\tLabel\tCommand\tParams");

        for (int i = 0; i < commands.size(); i++) {
            ArrayList<String> onecommand = commands.get(i);
            for (int j = 0; j < onecommand.size(); j++)
                System.out.print(onecommand.get(j) + (j == onecommand.size() - 1 ? "" : "\t"));
            System.out.println();
        }
    }

    static ArrayList<ArrayList<String>> getIntermediateCode( ) {
        return null;
    }

    public static void main(String[] args) {
        // Overall commands that will be read from the file
        ArrayList<ArrayList<String>> commands = new ArrayList<ArrayList<String>>();

        Map<String, OpTableValue> opTable = getOpTable();
        Map<String, Integer> labels = new HashMap<String, Integer>();

        ArrayList<String> temp = new ArrayList<String>();
        try {
            File file = new File("input.txt");
            Scanner reader = new Scanner(file);

            Integer counter = 0;
            while (reader.hasNextLine()) {
                String data = reader.nextLine();
                temp = new ArrayList<String>(Arrays.asList(data.split(" ")));

                // If the first word in the command is not present in the keywords
                if (opTable.containsKey(temp.get(0))) {
                    // Leave space for the labels
                    temp.add(0, "");
                } else {
                    // If the first word is not a keyword, then it is a label
                    labels.put(temp.get(0), counter);
                }
                // Get the starting address
                if (temp.get(1).trim().equals("START")) {
                    counter = Integer.parseInt(temp.get(2));
                }

                
                // If the command is not START or END, then increment the counter
                if (!temp.get(1).trim().equals("START") && !temp.get(1).trim().equals("END")) {
                    temp.add(0, counter.toString());
                    counter++;
                }

                // If the command is START or END then add a space
                if (temp.get(1).trim().equals("START") || temp.get(1).trim().equals("END")) {
                    temp.add(0, "");
                }
                commands.add(temp);
            }

            // Print the table
            printTable(commands);

            // Print the labels
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