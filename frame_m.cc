//
// Generated file, do not edit! Created by opp_msgtool 6.0 from frame.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wshadow"
#  pragma clang diagnostic ignored "-Wconversion"
#  pragma clang diagnostic ignored "-Wunused-parameter"
#  pragma clang diagnostic ignored "-Wc++98-compat"
#  pragma clang diagnostic ignored "-Wunreachable-code-break"
#  pragma clang diagnostic ignored "-Wold-style-cast"
#elif defined(__GNUC__)
#  pragma GCC diagnostic ignored "-Wshadow"
#  pragma GCC diagnostic ignored "-Wconversion"
#  pragma GCC diagnostic ignored "-Wunused-parameter"
#  pragma GCC diagnostic ignored "-Wold-style-cast"
#  pragma GCC diagnostic ignored "-Wsuggest-attribute=noreturn"
#  pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif

#include <iostream>
#include <sstream>
#include <memory>
#include <type_traits>
#include "frame_m.h"

namespace omnetpp {

// Template pack/unpack rules. They are declared *after* a1l type-specific pack functions for multiple reasons.
// They are in the omnetpp namespace, to allow them to be found by argument-dependent lookup via the cCommBuffer argument

// Packing/unpacking an std::vector
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::vector<T,A>& v)
{
    int n = v.size();
    doParsimPacking(buffer, n);
    for (int i = 0; i < n; i++)
        doParsimPacking(buffer, v[i]);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::vector<T,A>& v)
{
    int n;
    doParsimUnpacking(buffer, n);
    v.resize(n);
    for (int i = 0; i < n; i++)
        doParsimUnpacking(buffer, v[i]);
}

// Packing/unpacking an std::list
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::list<T,A>& l)
{
    doParsimPacking(buffer, (int)l.size());
    for (typename std::list<T,A>::const_iterator it = l.begin(); it != l.end(); ++it)
        doParsimPacking(buffer, (T&)*it);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::list<T,A>& l)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        l.push_back(T());
        doParsimUnpacking(buffer, l.back());
    }
}

// Packing/unpacking an std::set
template<typename T, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::set<T,Tr,A>& s)
{
    doParsimPacking(buffer, (int)s.size());
    for (typename std::set<T,Tr,A>::const_iterator it = s.begin(); it != s.end(); ++it)
        doParsimPacking(buffer, *it);
}

template<typename T, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::set<T,Tr,A>& s)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        T x;
        doParsimUnpacking(buffer, x);
        s.insert(x);
    }
}

// Packing/unpacking an std::map
template<typename K, typename V, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::map<K,V,Tr,A>& m)
{
    doParsimPacking(buffer, (int)m.size());
    for (typename std::map<K,V,Tr,A>::const_iterator it = m.begin(); it != m.end(); ++it) {
        doParsimPacking(buffer, it->first);
        doParsimPacking(buffer, it->second);
    }
}

template<typename K, typename V, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::map<K,V,Tr,A>& m)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        K k; V v;
        doParsimUnpacking(buffer, k);
        doParsimUnpacking(buffer, v);
        m[k] = v;
    }
}

// Default pack/unpack function for arrays
template<typename T>
void doParsimArrayPacking(omnetpp::cCommBuffer *b, const T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimPacking(b, t[i]);
}

template<typename T>
void doParsimArrayUnpacking(omnetpp::cCommBuffer *b, T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimUnpacking(b, t[i]);
}

// Default rule to prevent compiler from choosing base class' doParsimPacking() function
template<typename T>
void doParsimPacking(omnetpp::cCommBuffer *, const T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimPacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

template<typename T>
void doParsimUnpacking(omnetpp::cCommBuffer *, T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimUnpacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

}  // namespace omnetpp

Register_Class(frame)

frame::frame(const char *name, short kind) : ::omnetpp::cMessage(name, kind)
{
}

frame::frame(const frame& other) : ::omnetpp::cMessage(other)
{
    copy(other);
}

frame::~frame()
{
}

frame& frame::operator=(const frame& other)
{
    if (this == &other) return *this;
    ::omnetpp::cMessage::operator=(other);
    copy(other);
    return *this;
}

void frame::copy(const frame& other)
{
    this->source = other.source;
    this->destination = other.destination;
    this->hopCount = other.hopCount;
    this->type = other.type;
    this->dir = other.dir;
    this->freq = other.freq;
    for (size_t i = 0; i < 20; i++) {
        this->route[i] = other.route[i];
    }
    for (size_t i = 0; i < 100; i++) {
        this->data[i] = other.data[i];
    }
}

void frame::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cMessage::parsimPack(b);
    doParsimPacking(b,this->source);
    doParsimPacking(b,this->destination);
    doParsimPacking(b,this->hopCount);
    doParsimPacking(b,this->type);
    doParsimPacking(b,this->dir);
    doParsimPacking(b,this->freq);
    doParsimArrayPacking(b,this->route,20);
    doParsimArrayPacking(b,this->data,100);
}

void frame::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cMessage::parsimUnpack(b);
    doParsimUnpacking(b,this->source);
    doParsimUnpacking(b,this->destination);
    doParsimUnpacking(b,this->hopCount);
    doParsimUnpacking(b,this->type);
    doParsimUnpacking(b,this->dir);
    doParsimUnpacking(b,this->freq);
    doParsimArrayUnpacking(b,this->route,20);
    doParsimArrayUnpacking(b,this->data,100);
}

int frame::getSource() const
{
    return this->source;
}

void frame::setSource(int source)
{
    this->source = source;
}

int frame::getDestination() const
{
    return this->destination;
}

void frame::setDestination(int destination)
{
    this->destination = destination;
}

int frame::getHopCount() const
{
    return this->hopCount;
}

void frame::setHopCount(int hopCount)
{
    this->hopCount = hopCount;
}

int frame::getType() const
{
    return this->type;
}

void frame::setType(int type)
{
    this->type = type;
}

int frame::getDir() const
{
    return this->dir;
}

void frame::setDir(int dir)
{
    this->dir = dir;
}

int frame::getFreq() const
{
    return this->freq;
}

void frame::setFreq(int freq)
{
    this->freq = freq;
}

size_t frame::getRouteArraySize() const
{
    return 20;
}

int frame::getRoute(size_t k) const
{
    if (k >= 20) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)20, (unsigned long)k);
    return this->route[k];
}

void frame::setRoute(size_t k, int route)
{
    if (k >= 20) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)20, (unsigned long)k);
    this->route[k] = route;
}

size_t frame::getDataArraySize() const
{
    return 100;
}

int frame::getData(size_t k) const
{
    if (k >= 100) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)100, (unsigned long)k);
    return this->data[k];
}

void frame::setData(size_t k, int data)
{
    if (k >= 100) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)100, (unsigned long)k);
    this->data[k] = data;
}

class frameDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_source,
        FIELD_destination,
        FIELD_hopCount,
        FIELD_type,
        FIELD_dir,
        FIELD_freq,
        FIELD_route,
        FIELD_data,
    };
  public:
    frameDescriptor();
    virtual ~frameDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyName) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyName) const override;
    virtual int getFieldArraySize(omnetpp::any_ptr object, int field) const override;
    virtual void setFieldArraySize(omnetpp::any_ptr object, int field, int size) const override;

    virtual const char *getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const override;
    virtual std::string getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const override;
    virtual omnetpp::cValue getFieldValue(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual omnetpp::any_ptr getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const override;
};

Register_ClassDescriptor(frameDescriptor)

frameDescriptor::frameDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(frame)), "omnetpp::cMessage")
{
    propertyNames = nullptr;
}

frameDescriptor::~frameDescriptor()
{
    delete[] propertyNames;
}

bool frameDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<frame *>(obj)!=nullptr;
}

const char **frameDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *frameDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int frameDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 8+base->getFieldCount() : 8;
}

unsigned int frameDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_source
        FD_ISEDITABLE,    // FIELD_destination
        FD_ISEDITABLE,    // FIELD_hopCount
        FD_ISEDITABLE,    // FIELD_type
        FD_ISEDITABLE,    // FIELD_dir
        FD_ISEDITABLE,    // FIELD_freq
        FD_ISARRAY | FD_ISEDITABLE,    // FIELD_route
        FD_ISARRAY | FD_ISEDITABLE,    // FIELD_data
    };
    return (field >= 0 && field < 8) ? fieldTypeFlags[field] : 0;
}

const char *frameDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "source",
        "destination",
        "hopCount",
        "type",
        "dir",
        "freq",
        "route",
        "data",
    };
    return (field >= 0 && field < 8) ? fieldNames[field] : nullptr;
}

int frameDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "source") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "destination") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "hopCount") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "type") == 0) return baseIndex + 3;
    if (strcmp(fieldName, "dir") == 0) return baseIndex + 4;
    if (strcmp(fieldName, "freq") == 0) return baseIndex + 5;
    if (strcmp(fieldName, "route") == 0) return baseIndex + 6;
    if (strcmp(fieldName, "data") == 0) return baseIndex + 7;
    return base ? base->findField(fieldName) : -1;
}

const char *frameDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",    // FIELD_source
        "int",    // FIELD_destination
        "int",    // FIELD_hopCount
        "int",    // FIELD_type
        "int",    // FIELD_dir
        "int",    // FIELD_freq
        "int",    // FIELD_route
        "int",    // FIELD_data
    };
    return (field >= 0 && field < 8) ? fieldTypeStrings[field] : nullptr;
}

const char **frameDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *frameDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int frameDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    frame *pp = omnetpp::fromAnyPtr<frame>(object); (void)pp;
    switch (field) {
        case FIELD_route: return 20;
        case FIELD_data: return 100;
        default: return 0;
    }
}

void frameDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    frame *pp = omnetpp::fromAnyPtr<frame>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'frame'", field);
    }
}

const char *frameDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    frame *pp = omnetpp::fromAnyPtr<frame>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string frameDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    frame *pp = omnetpp::fromAnyPtr<frame>(object); (void)pp;
    switch (field) {
        case FIELD_source: return long2string(pp->getSource());
        case FIELD_destination: return long2string(pp->getDestination());
        case FIELD_hopCount: return long2string(pp->getHopCount());
        case FIELD_type: return long2string(pp->getType());
        case FIELD_dir: return long2string(pp->getDir());
        case FIELD_freq: return long2string(pp->getFreq());
        case FIELD_route: return long2string(pp->getRoute(i));
        case FIELD_data: return long2string(pp->getData(i));
        default: return "";
    }
}

void frameDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    frame *pp = omnetpp::fromAnyPtr<frame>(object); (void)pp;
    switch (field) {
        case FIELD_source: pp->setSource(string2long(value)); break;
        case FIELD_destination: pp->setDestination(string2long(value)); break;
        case FIELD_hopCount: pp->setHopCount(string2long(value)); break;
        case FIELD_type: pp->setType(string2long(value)); break;
        case FIELD_dir: pp->setDir(string2long(value)); break;
        case FIELD_freq: pp->setFreq(string2long(value)); break;
        case FIELD_route: pp->setRoute(i,string2long(value)); break;
        case FIELD_data: pp->setData(i,string2long(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'frame'", field);
    }
}

omnetpp::cValue frameDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    frame *pp = omnetpp::fromAnyPtr<frame>(object); (void)pp;
    switch (field) {
        case FIELD_source: return pp->getSource();
        case FIELD_destination: return pp->getDestination();
        case FIELD_hopCount: return pp->getHopCount();
        case FIELD_type: return pp->getType();
        case FIELD_dir: return pp->getDir();
        case FIELD_freq: return pp->getFreq();
        case FIELD_route: return pp->getRoute(i);
        case FIELD_data: return pp->getData(i);
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'frame' as cValue -- field index out of range?", field);
    }
}

void frameDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    frame *pp = omnetpp::fromAnyPtr<frame>(object); (void)pp;
    switch (field) {
        case FIELD_source: pp->setSource(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_destination: pp->setDestination(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_hopCount: pp->setHopCount(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_type: pp->setType(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_dir: pp->setDir(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_freq: pp->setFreq(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_route: pp->setRoute(i,omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_data: pp->setData(i,omnetpp::checked_int_cast<int>(value.intValue())); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'frame'", field);
    }
}

const char *frameDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

omnetpp::any_ptr frameDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    frame *pp = omnetpp::fromAnyPtr<frame>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void frameDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    frame *pp = omnetpp::fromAnyPtr<frame>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'frame'", field);
    }
}

namespace omnetpp {

}  // namespace omnetpp

