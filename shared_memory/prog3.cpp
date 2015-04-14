/*/ Author: Lisa /*/

#include "prog3.h"


/***************************************************************************
 * Function:    mailbox_init
 *
 * Description: mailbox initialization function
 *
 * Input:       command - argument vector containing the number of mailboxes
 *                        and the size of each mailbox
 *
 * Output:      none
 * 
***************************************************************************/
void mailbox_init(vector<string> & command)
{
	// Error checking
    if( command.size() != 3    || 
        !check_num(command[1]) || !check_num(command[2]) ) 
    {
        cout << "Usage: mboxinit <number of mailboxes> "
             << "<size of mailbox in kbytes>" << endl;
        return;
    }
    
    // Get number of mailboxes and mailbox size from arguments
    int num = atoi(command[1].c_str());
    int size = atoi(command[2].c_str());

	// Header block size (holds size of mailboxes + number of mailboxes)
	int header_size = (sizeof(int)*2);

	// Data block size (in kilobytes)
	int data_size = num * size * 1024;

	// Get shared memory id
	int shmid = shmget(SHMKEY, header_size + data_size, IPC_CREAT | IPC_EXCL | 0666);

	if (shmid < 0)
	{
		cout << "Error: Failed to set up shared memory block "
		     << "or a shared memory block has already been initialized" << endl;
		return;
	}
 
	// Create header:

		// Set pointer to start of header
		int *headerp = (int*)shmat(shmid, 0, 0); 

		// Write mailbox number and size to header
		*headerp = num;
		*(headerp + 1) = size; 

	// Release shared memory
	shmdt(headerp);

}

/***************************************************************************
 * Function:    mailbox_remove
 *
 * Description: mailbox removal function
 *
 * Input:       none
 *
 * Output:      none
 * 
***************************************************************************/
void mailbox_remove()
{
	// Get shared memory id using key
	int shmid = shmget(SHMKEY, 0,0);

	// Check if shared memory block has been initialized
	if ( shmid < 0)
	{
		cout << "Error: Request could not be completed - Shared memory " 
			 << "block has not been initialized" << endl;
		return;
	}

	// Remove mailboxes
	shmctl(shmid, IPC_RMID, 0);

}


/**************************************************************************
 * Function:    mailbox_write
 *
 * Description: write data to a mailbox
 *
 * Input:       command - argument vector containing the index number of 
 *                        the mailbox to which data is to be written.
 *
 * Output:      none
 * 
**************************************************************************/
void mailbox_write(vector<string> & command)
{
	// Error checking
    if( command.size() != 2 || !check_num(command[1]) )
    {
    	cout << "Usage: mboxwrite <boxnum>" << endl;
    	return;
    }

    // Get mailbox number 
    int index = atoi(command[1].c_str());

	// Get shared memory id using key
	int shmid = shmget(SHMKEY, 0,0);

	// Check if shared memory block has been initialized
	if ( shmid < 0)
	{
		cout << "Error: Request could not be completed - Shared memory " 
			 << "block has not been initialized" << endl;
		return;
	}

	// Get data to be written
    string msg, line;
    cout << "Enter data to be written: ";
    while (getline(cin, line))
		msg.append(line);
	cout << endl;
	cin.clear();

	// Set pointer to start of header
	int *headerp = (int*)shmat(shmid, 0, 0); 

	// Get number of mailboxes from header
	int num = *headerp;

	// Get size of each mailbox from header
	int size = *(headerp + 1);

	// Get mailbox pointer
	char *boxp = (char*)headerp + sizeof(int)*2 + (size*1024)*index;

	// Error check
	if (index >= num || index < 0)
	{
		cout << "Error: Invalid mailbox index" << endl;
	}
	else
	{
		// If data is larger than the mailbox, truncate message and flag user 
		if (msg.length() > ((size * 1024) - 1))
		{
			memcpy(boxp, msg.c_str(), ((size * 1024) - 1));
			boxp[((size * 1024) - 1)] = '\0';

		}
		else  // Else, write entire message to mailbox
			memcpy(boxp, msg.c_str(), msg.length());

	}

	// Release shared memory
	shmdt(headerp);

}


/**************************************************************************
 * Function:    mailbox_read
 *
 * Description: read data from a mailbox
 *
 * Input:       command - argument vector containing the index number of 
 *                        the mailbox from which data is to be read.
 *
 * Output:      none
 * 
**************************************************************************/
void mailbox_read(vector<string> & command)
{
	// Error checking
    if( command.size() != 2 || !check_num(command[1]) ) 
    {
    	cout << "Usage: mboxread <boxnum>" << endl;
    	return;
    }
    
    // Get mailbox number
    int index = atoi(command[1].c_str());
	
	// Get shared memory id using key
	int shmid = shmget(SHMKEY, 0,0);

	// Check if shared memory block has been initialized
	if ( shmid < 0)
	{
		cout << "Error: Request could not be completed - Shared memory " 
			 << "block has not been initialized" << endl;
		return;
	}

	// Set pointer to start of header
	int *headerp = (int*)shmat(shmid, 0, 0); 

	// Get number of mailboxes from header
	int num = *headerp;

	// Get size of each mailbox from header
	int size = *(headerp + 1);

	
	// Error check
	if (index >= num || index < 0)
	{
		cout << "Error: Invalid mailbox index" << endl;

	}
	else
	{
		// Get mailbox pointer
		char *boxp = (char*)headerp + sizeof(int)*2 + (size*1024)*index;

		// Read message string
		cout << boxp << endl;
	}


	// Release shared memory
	shmdt(headerp);
}


/**************************************************************************
 * Function:    mailbox_copy
 *
 * Description: copy data from one mailbox to another
 *
 * Input:       command - argument vector containing the index numbers of 
 *                        the two mailboxes involved in the copy transaction
 *
 * Output:      none
 * 
**************************************************************************/
void mailbox_copy(vector<string> & command)
{
	// Error checking
	if( command.size() != 3    || 
	    !check_num(command[1]) || !check_num(command[2]) ) 
	{
	    cout << "Usage: mboxcopy <boxnum1> <boxnum2>" << endl;
	    return;
	}
	
	// Get index numbers of the two mailboxes
	int index_from = atoi(command[1].c_str());
	int index_to = atoi(command[2].c_str());
	
	// Get shared memory id using key
	int shmid = shmget(SHMKEY, 0,0);

	// Check if shared memory block has been initialized
	if ( shmid < 0)
	{
		cout << "Error: Request could not be completed - Shared memory " 
			 << "block has not been initialized" << endl;
		return;
	}

	// Set pointer to start of header
	int *headerp = (int*)shmat(shmid, 0, 0); 

	// Get number of mailboxes from header
	int num = *headerp;

	// Get size of each mailbox from header
	int size = *(headerp + 1);

	// Get mailbox pointers
	char *boxp_to = (char*)headerp + sizeof(int)*2 + (size*1024)*index_from;
	char *boxp_from = (char*)headerp + sizeof(int)*2 + (size*1024)*index_to;

	// Error check
	if (index_from >= num || index_from < 0 || 
		  index_to >= num || index_to < 0 || 
		  index_to == index_from)
	{
		cout << "Error: Invalid mailbox index" << endl;
	}
	else
	{
		// Copy data from one mailbox to the other
		strcpy(boxp_to, boxp_from);
	}


	// Release shared memory
	shmdt(headerp);

}

/**************************************************************************
 * Function:    check_num
 *
 * Description: check if string is an integer
 *
 * Input:       num - string to be tested
 *
 * Output:      true if string is an integer, false if it is not an integer
 * 
**************************************************************************/
bool check_num(string num)
{
    for(string::const_iterator k = num.begin(); k != num.end(); ++k)
    {
        if( isdigit(*k) == false )
            return false;
    }

    return true;
}

