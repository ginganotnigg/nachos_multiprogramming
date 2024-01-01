#include "ptable.h"
#include "main.h"
#include "synch.h"
#include <iostream>


PTable::PTable(int size) {
    psize = size;
    for (int i = 0; i < MAX_PROCESS; i++) {
        pcb[i] = NULL;
    }
    bm = new Bitmap(size);
    bmsem = new Semaphore("bmsem", 1);
    pcb[0] = new PCB(0, "MainThread");
    if (kernel->currentThread->processID == 0) {
        pcb[0]->SetThread(kernel->currentThread);    
    }
}

PTable::PTable() {
    psize = MAX_PROCESS;
     for (int i = 0; i < MAX_PROCESS; i++) {
        pcb[i] = NULL;
    }
    bm = new Bitmap(MAX_PROCESS);
    bmsem = new Semaphore("bmsem", 1);
    pcb[0] = new PCB(0, "MainThread");
    if (kernel->currentThread->processID == 0) {
        pcb[0]->SetThread(kernel->currentThread);    
    }
}

PTable::~PTable() {
    for (int i = 0; i < psize; i++) {
        if (!pcb[i]) delete pcb[i];
    }
    delete bmsem;
}

char* PTable::GetFileName(int id) { return pcb[id]->GetFileName(); }

int PTable::ExecUpdate(char* name) {
   //mutex để chỉ cho chương trình cấp phát chạy
    bmsem->P();

    //kiểm tra lần nữa tham số name có truyền vào được không
    if (name == NULL) {
        bmsem->V();
        return -1;
    }
    // So sánh tên chương trình và tên của currentThread để chắc chắn rằng
    // chương trình này không gọi thực thi chính nó.
    if (strcmp(name, kernel->currentThread->getName()) == 0) {
        bmsem->V();
        return -1;
    }

    // Tìm slot trống trong bảng Ptable.
    int index = this->GetFreeSlot();

    // Đã có đủ MAX_PROCESS đang chạy -> không còn slot trống
    if (index < 0) {
        bmsem->V();
        return -1;
    }
    std::cout <<"index cap phat: " << index << std::endl;

    // Nếu có slot trống thì khởi tạo một PCB mới với processID chính là index
    // của slot này và name la name file
    pcb[index] = new PCB(index, name);

    // parrentID là processID của currentThread
    pcb[index]->parentID = kernel->currentThread->processID;

    // Gọi thực thi phương thức Exec của lớp PCB.
    int pid = pcb[index]->Exec(name);

    // Gọi bmsem->V()
    bmsem->V();
    // Trả về kết quả thực thi của PCB->Exec.
    return pid;
}

int PTable::ExecV(int argc, char** argv) {
   //mutex để chỉ cho chương trình cấp phát chạy
    bmsem->P();

    // Tìm slot trống trong bảng Ptable.
    int index = this->GetFreeSlot();

    // Đã có đủ MAX_PROCESS đang chạy -> không còn slot trống
    if (index < 0) {
        bmsem->V();
        return -1;
    }
    //cout <<"index cap phat: " << index << endl;

    // Nếu có slot trống thì khởi tạo một PCB mới với processID chính là index
    // của slot này và name la name file
    pcb[index] = new PCB(index, argv[0]);

    // parrentID là processID của currentThread
    pcb[index]->parentID = kernel->currentThread->processID;

    // Gọi thực thi phương thức Exec của lớp PCB.
    int pid = pcb[index]->ExecV(argc, argv);

    // Gọi bmsem->V()
    bmsem->V();
    // Trả về kết quả thực thi của PCB->Exec.
    return pid;
}

int PTable::JoinUpdate(int id) {
    // cout <<"thread call Join: "<< kernel->currentThread->processID << endl;
    // Ta kiểm tra tính hợp lệ của processID id và kiểm tra tiến trình gọi Join
    // có phải là cha của tiến trình có processID là id hay không. Nếu không
    // thỏa, ta báo lỗi hợp lý và trả về -1.
    if (id < 0 || id >= psize || pcb[id] == NULL ||
        pcb[id]->parentID != kernel->currentThread->processID) {
        return -1;
    }

    // Tăng numwait và gọi JoinWait() để chờ tiến trình con thực hiện.
    pcb[pcb[id]->parentID]->IncNumWait();
    pcb[id]->JoinWait();

    // Sau khi tiến trình con thực hiện xong, tiến trình đã được giải phóng
    
    // Xử lý exitcode.
    int exitcode = pcb[id]->GetExitCode();
    // ExitRelease() để cho phép tiến trình con thoát.
    pcb[id]->ExitRelease();

    return exitcode;
}

void PTable::ExitUpdate(int exitcode) {
    // Nếu tiến trình gọi là main process thì gọi Halt().
    //cout <<"thread call exit: "<< kernel->currentThread->processID << endl;
    int id = kernel->currentThread->processID;
    if (id == 0) {
        kernel->currentThread->FreeSpace();
        kernel->interrupt->Halt();
    }

    if (pcb[id]== NULL) {
        return;
    }

    // Ngược lại gọi SetExitCode để đặt exitcode cho tiến trình gọi.
    pcb[id]->SetExitCode(exitcode);
    pcb[pcb[id]->parentID]->DecNumWait();

    // Gọi JoinRelease để giải phóng tiến trình cha đang đợi nó (nếu có)
    // và ExitWait() để xin tiến trình cha cho phép thoát.
    
    pcb[id]->JoinRelease();
    pcb[id]->ExitWait();
    Remove(id);
}

int PTable::GetFreeSlot()
{
	return bm->FindAndSet();
}

// Check if Process ID is Exist
bool PTable::IsExist(int pid)
{
	return bm->Test(pid);
}

// Remove proccess ID out of table
// When it ends
void PTable::Remove(int pid)
{
	bm->Clear(pid);
	if(pcb[pid] != NULL)
		delete pcb[pid];
}



