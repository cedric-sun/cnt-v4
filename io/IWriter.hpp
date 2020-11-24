// cesun, 11/23/20 3:28 PM.

#ifndef CNT5106_V4_IWRITER_HPP
#define CNT5106_V4_IWRITER_HPP


class IWriter {
public:
    virtual void write(const void *buf, int length) = 0;

    virtual ~IWriter() = default;
};


#endif //CNT5106_V4_IWRITER_HPP
