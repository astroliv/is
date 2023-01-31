////该文件已废弃
//#include "instream.h"
//#include "../report/ireport.h"
//
//Instream::Instream(ByteStream *bst) : byteStream(bst) {}
//
//void Instream::write(Bytecode type) const {
//	assert(byteStream != nullptr, "byteStream = nullptr!");
//	byteStream->append((byte) type);
//}
//
//void Instream::write(byte operand) const {
//	assert(byteStream != nullptr, "byteStream = nullptr!");
//	byteStream->append(operand);
//}
//
//void Instream::write(Bytecode type, byte operand) const {
//	write(type);
//	write(operand);
//}
//
//void Instream::write(int64_t operand, int8_t len) const {
//	assert(byteStream != nullptr, "byteStream = nullptr!");
//	switch (len) {//直接穷举,没必要整循环
//		case -1:
//			writeVarg(operand);
//			return;
//		case 1:
//			byteStream->append(operand);
//			return;
//		case 2:
//			byteStream->append(operand);
//			byteStream->append(operand >> 8);
//			return;
//		case 4:
//			byteStream->append(operand);
//			byteStream->append(operand >> 8);
//			byteStream->append(operand >> 16);
//			byteStream->append(operand >> 24);
//			return;
//		case 8:
//			byteStream->append(operand);
//			byteStream->append(operand >> 8);
//			byteStream->append(operand >> 16);
//			byteStream->append(operand >> 24);
//			byteStream->append(operand >> 32);
//			byteStream->append(operand >> 40);
//			byteStream->append(operand >> 48);
//			byteStream->append(operand >> 56);
//			return;
//		default:
//			unreachableBranch();
//	}
//}
//
//void Instream::write(Bytecode type, int64_t operand, int8_t len) const {
//	write(type);
//	write(operand, len);
//}
//
//int64_t Instream::read(isize idx) {
//	advLen = 1;//步过字节码
//	return read(idx + 1,
//	            BytecodeInfoList[byteStream->get(idx)].fopLen[0]);
//}
//
//int64_t Instream::read(isize idx, int8_t operandLen) {
//	assert(byteStream != nullptr, "byteStream = nullptr!");
//	advLen += operandLen;
//	switch (operandLen) {
//		case -1:
//			++advLen;//前面加-1,这里加回来
//			return (int64_t) readVarg(idx);
//		case 1:
//			return *byteStream->getPtr(idx);
//		case 2:
//			return *(int16_t *) byteStream->getPtr(idx);
//		case 4:
//			return *(int32_t *) byteStream->getPtr(idx);
//		case 8:
//			return *(int64_t *) byteStream->getPtr(idx);
//		default:
//			unreachableBranch();
//	}
//	return 0;
//}
//
//void Instream::writeVarg(uint64_t arg) const {
//	assert(byteStream != nullptr, "The byteStream pointer is nullptr!");
//	uint8_t i = 0;
//	do {
//		byteStream->append((arg >> 7) && (i < 8) ? arg & 0x7F | 0x80 : arg);
//	} while ((arg >>= 7) && (i++ < 8));
//}
//
//uint64_t Instream::readVarg(isize idx) {
//	assert(byteStream != nullptr, "The byteStream pointer is nullptr!");
//	uint64_t result = 0;
//	uint8_t tmp, i = 0;
//	do {
//		tmp = byteStream->get(idx + i);
//		result |= (i < 8 ? tmp & 0x7F : tmp) << (i * 7);
//	} while ((tmp >> 7) && (i++ < 8));
//	advLen += i + 1;//增大步进长度
//	return result;
//}
//
//string Instream::dump(isize idx) {
//	assert(byteStream != nullptr, "The byteStream pointer is nullptr!");
//	uint64_t args[2] = {0};           //字节码的参数
//	char buffer[64] = {0};            //创建缓冲区
//	byte in = byteStream->get(idx);   //指令的索引(instruction)
//	const char *fmt = nullptr;        //格式化文本
//	switch (BytecodeInfoList[in].opNum) { //根据参数个数分情况设定
//		case 0://无参字节码
//			advLen = 1;
//			fmt = "%-5u: %s";
//			break;
//		case 1://单参字节码
//			fmt = "%-5u: %s %llu";
//			args[0] = read(idx);
//			break;
//		case 2://双参字节码
//			fmt = "%-5u: %s %llu,%llu";
//			args[0] = read(idx);//读第一个参数
//			args[1] = read(idx + advLen, BytecodeInfoList[in].fopLen[1]);//读第二个参数
//			break;
//		default:
//			unreachableBranch();
//			break;
//	}
//	sprintf(buffer, fmt, idx, BytecodeInfoList[in].strName, args[0], args[1]);
//	return string(buffer);
//}
//
