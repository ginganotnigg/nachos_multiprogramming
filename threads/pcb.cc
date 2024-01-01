#include "pcb.h"
#include "synch.h"

//extern void StartProcess_2(int id);

PCB::PCB(int pid)
{
    if (pid == 0) {
        this->parentID = -1;
    } 
    else this->parentID = kernel->currentThread->processID;

	this->numwait = this->exitcode = 0;
	this->thread = NULL;
	this->joinsem = new Semaphore("joinsem", 0);
	this->exitsem = new Semaphore("exitsem", 0);
	this->mutex = new Semaphore("mutex", 1);
}

PCB::PCB(int pid, char* fn) : PCB(pid) {
    SetFileName(fn);
}

PCB::~PCB()
{
	
	if(joinsem != NULL) delete this->joinsem;
	if(exitsem != NULL) delete this->exitsem;
	if(mutex != NULL) delete this->mutex;
	if(thread != NULL) {	
        if(thread->space != 0) delete thread->space;	
		//thread->FreeSpace();
		thread->Finish();
	}
}
int PCB::GetID(){ return this->thread->processID; }
int PCB::GetNumWait() { return this->numwait; }

int PCB::GetExitCode() { return this->exitcode; }
void PCB::SetExitCode(int ec){ this->exitcode = ec; }

// Process tranlation to block
// Wait for JoinRelease to continue exec
void PCB::JoinWait()
{
	//Gọi joinsem->P() để tiến trình chuyển sang trạng thái block và ngừng lại, chờ JoinRelease để thực hiện tiếp.
    joinsem->P();
}

// JoinRelease process calling JoinWait
void PCB::JoinRelease()
{ 
	// Gọi joinsem->V() để giải phóng tiến trình gọi JoinWait().
    joinsem->V();
}

// Let process tranlation to block state
// Release wating process
void PCB::ExitRelease() 
{
	// Gọi exitsem-->V() để giải phóng tiến trình đang chờ.
    exitsem->V();
}

// Waiting for ExitRelease to continue exec
void PCB::ExitWait()
{ 
	// Gọi exitsem-->V() để tiến trình chuyển sang trạng thái block và ngừng lại, chờ ExitReleaseđể thực hiện tiếp.
    exitsem->P();
}

void PCB::IncNumWait()
{
	mutex->P();
	++numwait;
	mutex->V();
}

void PCB::DecNumWait()
{
	mutex->P();
	if(numwait > 0) --numwait;
	mutex->V();
}

char* PCB::GetFileName() { return this->fileName; }
void PCB::SetFileName(char* fn){ strcpy(fileName, fn);}

void StartProcess_1(void* pid) {
    int id;
    id = *((int*)pid);
    //cout << "start process: " << id << endl;
    // Lay fileName cua process id nay
    char* fileName = kernel->pTab->GetFileName(id);
    AddrSpace* space;
    space = new AddrSpace(fileName);
    //space->Load(fileName);

    if (space == NULL) {
        printf("\nPCB::Exec: Can't create AddSpace.");
        return;
    }
    space->Execute();
    kernel->currentThread->space = space;

    // space->InitRegisters();	// set the initial register values
    // space->RestoreState();	// load page table register

    // kernel->machine->Run();	// jump to the user progam
    ASSERT(FALSE);  // machine->Run never returns;
                    // the address space exits
                    // by doing the syscall "exit"
}

void StartProcess_2(void* argv) {
    char** arg= (char**)(argv);
    //cout << "start process: " << id << endl;
    // Lay fileName cua process id nay
    char* fileName = arg[0];
    AddrSpace* space;
    space = new AddrSpace(fileName);
    //space->Load(fileName);

    if (space == NULL) {
        printf("\nPCB::Exec: Can't create AddSpace.");
        return;
    }
    space->Execute();
    // kernel->currentThread->space = space;

    // space->InitRegisters();	// set the initial register values
    // space->RestoreState();	// load page table register

    // kernel->machine->Run();	// jump to the user progam
    ASSERT(FALSE);  // machine->Run never returns;
                    // the address space exits
                    // by doing the syscall "exit"
}

int PCB::Exec(char* fileName)
{  
    // Gọi mutex->P(); để giúp tránh tình trạng nạp 2 tiến trình cùng 1 lúc.
	mutex->P();

    // Kiểm tra thread đã khởi tạo thành công chưa, nếu chưa thì báo lỗi là không đủ bộ nhớ, gọi mutex->V() và return.             
	this->thread = new Thread(fileName);

	if(this->thread == NULL){
		printf("\nPCB::Exec:: Not enough memory!\n");
        mutex->V();
		return -1;
	}
	DEBUG(dbgSys, "Create the thread successfully");

	// Đặt parrentID của thread này là processID của thread gọi thực thi Exec
	this->parentID = kernel->currentThread->processID;
	// Gọi thực thi Fork(StartProcess_1, id) => Ta cast thread thành kiểu int, sau đó khi xử ký hàm StartProcess ta cast Thread về đúng kiểu của nó.
 	this->thread->Fork(StartProcess_1, &(this->thread->processID));

    mutex->V();
	return this->thread->processID;

}

int PCB::ExecV(int argc, char** argv) {
    // cerr << filename << ' ' << pid << endl;
    mutex->P();
    //cout << filename << " " << processID <<  endl;
    this->thread = new Thread(argv[0]);
    //cout <<"thread: " << processID << endl;
    if (this->thread == NULL) {
        printf("\nPCB::Exec: Not enough memory!\n");
        mutex->V();  // Nha CPU de nhuong CPU cho tien trinh khac
        return -1;    // Tra ve -1 neu that bai
    }
    // Gọi thực thi Fork(StartProcess_1, id) => Ta cast thread thành kiểu int,
    // sau đó khi xử ký hàm StartProcess ta cast Thread về đúng kiểu của nó.
    // Không được sử dụng biến id ở đây, vì biến id là biến cục bộ,
    // nên khi hàm này kết thúc thì giá trị của biến này cũng bị xóa
    this->thread->Fork(StartProcess_2, argv);
    mutex->V();
    // Trả về id.
    return this->thread->processID;
}