#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_WORDS_IN_SENTENCE_GENERATION 20
#define MAX_WORD_LENGTH 100
#define MAX_SENTENCE_LENGTH 1000
#define SUCCESS 0
#define FAILURE 1
#define BASE 10
#define FIVE 5
#define FOUR 4
#define THREE 3
#define TWO 2
#define ONE 1
#define ZERO 0
#define TRUE 0
#define NEGATIVE (-1)
#define FILE_ERROR "Error: File failed to open\n"
#define ALLOCATION_FAILURE "Allocation failure: Failed to allocate memory\n"

typedef struct WordStruct {
  char *word;
  struct WordProbability *prob_list;
  int len_of_prob_list;
  int amount;
} WordStruct;

typedef struct WordProbability {
  struct WordStruct *word_struct_ptr;
  int amount;

} WordProbability;

/************ LINKED LIST ************/
typedef struct Node {
  WordStruct *data;
  struct Node *next;
} Node;

typedef struct LinkList {
  Node *first;
  Node *last;
  int size;
} LinkList;

/**
 * Add data to new node at the end of the given link list.
 * @param link_list Link list to add data to
 * @param data pointer to dynamically allocated data
 * @return 0 on success, 1 otherwise
 */
int add(LinkList *link_list, WordStruct *data)
{
  Node *new_node = malloc(sizeof(Node));
  if (new_node == NULL)
  {
	return 1;
  }
  *new_node = (Node){data, NULL};

  if (link_list->first == NULL)
  {
	link_list->first = new_node;
	link_list->last = new_node;
  }
  else
  {
	link_list->last->next = new_node;
	link_list->last = new_node;
  }

  link_list->size++;
  return 0;
}
/*************************************/

/**
 * Get random number between 0 and max_number [0, max_number).
 * @param max_number
 * @return Random number
 */
int get_random_number(int max_number)
{
  return rand() % max_number;
}

/**
 * Choose randomly the next word from the given dictionary, drawn uniformly.
 * The function won't return a word that end's in full stop '.' (Nekuda).
 * @param dictionary Dictionary to choose a word from
 * @return WordStruct of the chosen word
 */
WordStruct *get_first_random_word(LinkList *dictionary)
{
  int counter;
  int rand_num;
  while (FAILURE)
    {
      counter = ZERO;
      rand_num = get_random_number (dictionary->size);
      Node *current = dictionary->first;
      while (counter != rand_num)
        {
          counter++;
          current = current->next;
        }
      if (strcmp (&current->data->word[strlen (current->data->word)
                                       - ONE], ".") != TRUE)
        {
          return current->data;
        }
    }
}


/**
 * Choose randomly the next word. Depend on it's occurrence frequency
 * in word_struct_ptr->WordProbability.
 * @param word_struct_ptr WordStruct to choose from
 * @return WordStruct of the chosen word
 */
WordStruct *get_next_random_word(WordStruct *word_struct_ptr)
{
  int rand_num = get_random_number (word_struct_ptr->amount);
  int index = ZERO;
  int counter = word_struct_ptr->prob_list[index].amount;
  while (counter < rand_num)
    {
      index ++;
      counter += word_struct_ptr->prob_list[index].amount;
    }
  return word_struct_ptr->prob_list[index].word_struct_ptr;
}

/**
 * Receive dictionary, generate and print to stdout random sentence out of it.
 * The sentence most have at least 2 words in it.
 * @param dictionary Dictionary to use
 * @return Amount of words in printed sentence
 */
int generate_sentence(LinkList *dictionary)
{
  int amount = ONE;
  WordStruct *current_word = get_first_random_word(dictionary);
  printf("%s ", current_word->word);
  while (current_word->prob_list != NULL && (amount !=
  MAX_WORDS_IN_SENTENCE_GENERATION))
    {
      amount ++;
      current_word = get_next_random_word(current_word);
      printf("%s ", current_word->word);
    }
  printf("\n");
  return amount;
}

int check_word_in_prob_list(WordStruct *first_word, WordStruct *second_word)
{
  for (int i=ZERO; i<first_word->len_of_prob_list; i++)
    {
      if (first_word->prob_list[i].word_struct_ptr == second_word)
        {
          first_word->prob_list[i].amount ++;
          return SUCCESS;
        }
    }
  return FAILURE;
}
/**
 * Gets 2 WordStructs. If second_word in first_word's prob_list,
 * update the existing probability value.
 * Otherwise, add the second word to the prob_list of the first word.
 * @param first_word
 * @param second_word
 * @return 0 if already in list, 1 otherwise.
 */
void add_word_to_probability_list(WordStruct *first_word,
                                 WordStruct *second_word)
{
  if (strcmp(&first_word->word[strlen(first_word->word)-ONE], ".") == ZERO)
    {
      return;
    }
  if (first_word->prob_list == NULL)
    {
      WordProbability *prob = (WordProbability*) calloc(ONE, sizeof
          (WordProbability));
      if (prob == NULL)
        {
          printf(ALLOCATION_FAILURE);
          exit(EXIT_FAILURE);
        }
      first_word->prob_list = prob;
      first_word->prob_list->word_struct_ptr = second_word;
      first_word->prob_list->amount = ONE;
      first_word->len_of_prob_list ++;
      return;
    }
  else if (check_word_in_prob_list(first_word, second_word) == SUCCESS)
    {
      return;
    }
  first_word->len_of_prob_list ++;
  WordProbability *temp = (WordProbability*) realloc(first_word->prob_list,
        first_word->len_of_prob_list * sizeof(WordProbability));
  if (temp == NULL)
    {
      printf(ALLOCATION_FAILURE);
      exit(EXIT_FAILURE);
    }
  first_word->prob_list = temp;
  first_word->prob_list[first_word->len_of_prob_list-ONE].word_struct_ptr =
      second_word;
  first_word->prob_list[first_word->len_of_prob_list-ONE].amount = ONE;
}

WordStruct * word_in_dictionary(char* word, LinkList *dictionary)
{
  Node *current = dictionary->first;
  if (current == NULL)
    {
      return NULL;
    }
  for(int i=ZERO; i < dictionary->size; i++)
    {
      if(strcmp(current->data->word, word) == SUCCESS)
        {
          current->data->amount += ONE;
          return current->data;
        }
      current = current->next;
    }
  return NULL;
}
/**
 * Read word from the given file. Add every unique word to the dictionary.
 * Also, at every iteration, update the prob_list of the previous word with
 * the value of the current word.
 * @param fp File pointer
 * @param words_to_read Number of words to read from file.
 *                      If value is bigger than the file's word count,
 *                      or if words_to_read == -1 than read entire file.
 * @param dictionary Empty dictionary to fill
 */
void fill_dictionary(FILE *fp, int words_to_read, LinkList *dictionary)
{
  char sentence[MAX_SENTENCE_LENGTH];
  int counter = ZERO;
  char* word;
  WordStruct *previous_word = NULL;
  while(fgets(sentence, MAX_SENTENCE_LENGTH, fp) != NULL)
    {
      word = strtok(sentence, " \n");
      while (word != NULL && counter != words_to_read)
        {
          counter ++;
          WordStruct *already_in_dict = word_in_dictionary(word, dictionary);
          if (already_in_dict != NULL)
            {
              add_word_to_probability_list(previous_word, already_in_dict);
              previous_word = already_in_dict;
              word = strtok (NULL, " \n");
              continue;
            }
          WordStruct *temp = (WordStruct *) calloc (ONE, sizeof(WordStruct));
          if (temp == NULL)
            {
              printf(ALLOCATION_FAILURE);
              exit(EXIT_FAILURE);
            }
          char *new_word = (char *) malloc ((strlen(word)+ ONE)*sizeof(char));
          if (new_word == NULL)
            {
              printf(ALLOCATION_FAILURE);
              exit(EXIT_FAILURE);
            }
          temp->word = new_word;
          strcpy(temp->word, word);
          temp->amount ++;
          if (previous_word != NULL)
            {
              add_word_to_probability_list(previous_word, temp);
            }
          previous_word = temp;
          if (add(dictionary, temp) == FAILURE)
            {
              printf(ALLOCATION_FAILURE);
              exit(EXIT_FAILURE);
            }
          word = strtok (NULL, " \n");
        }
      if(counter == words_to_read)
        {
          break;
        }
    }
}

/**
 * Free the given dictionary and all of it's content from memory.
 * @param dictionary Dictionary to free
 */
void free_dictionary(LinkList *dictionary)
{
  Node* current = dictionary->first;
  for (int j=ZERO; j<dictionary->size; j++)
    {
      free(current->data->prob_list);
      free(current->data->word);
      free(current->data);
      Node* temp = current;
      current = current->next;
      free(temp);
    }
}

int check_inputs(int argc)
{
  if (argc != FIVE && argc != FOUR)
    {
      printf("Usage: Please enter <seed value> <tweet amount> <text file> "
             "<word amount>\n");
      return FAILURE;
    }
  return SUCCESS;
}

int check_file(FILE *file)
{
  if (file == NULL)
    {
      printf("Error: The given file does not exist or access was denied");
      return FAILURE;
    }
  return SUCCESS;
}

/**
 * @param argc
 * @param argv 1) Seed
 *             2) Number of sentences to generate
 *             3) Path to file
 *             4) Optional - Number of words to read
 */
int main(int argc, char *argv[])
{
  if (check_inputs(argc) == FAILURE)
  {
    return EXIT_FAILURE;
  }
  FILE *file = fopen(argv[THREE], "r");
  if (check_file(file) == FAILURE)
  {
    printf(FILE_ERROR);
    return EXIT_FAILURE;
  }
  char *ptr;
  LinkList dictionary={NULL, NULL, ZERO};
  if (argc == FOUR)
    {
      fill_dictionary(file,  NEGATIVE, &dictionary);
    }
  else
    {
      fill_dictionary(file,  (int) strtol(argv[FOUR],&ptr,BASE), &dictionary);
    }
  srand((unsigned int) strtol(argv[ONE],&ptr,BASE));
  for (int i=ONE; i<=(int) strtol(argv[TWO],&ptr,BASE); i++)
    {
      printf("Tweet %d: ", i);
      generate_sentence(&dictionary);
    }
  free_dictionary(&dictionary);
  return 0;
}
