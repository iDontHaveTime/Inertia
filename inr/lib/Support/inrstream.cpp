/* Inertia's includes. */
#include "inr/Support/inrstream.hpp"

namespace inr{

/* This is the implementation for the inr::endl manipulator. */
inr_ostream& _inr_endl_implementation(inr_ostream& _stream){
    (_stream<<'\n').flush();
    return _stream;
}

inr_ostream& _inr_flush_implementation(inr_ostream& _stream){
    _stream.flush();
    return _stream;
}

/* Initialize the stdout inr_ostream. */
inr_ostream out = {stdout};
/* Initialize the stderr inr_ostream. */
inr_ostream err = {stderr};

/* Initialize the stdin inr_istream. */
inr_istream in = {stdin};

/* Initialize the stream endl manipulator. */
inr_stream_manipulator endl = _inr_endl_implementation;
/* Initialize the stream flush manipulator. */
inr_stream_manipulator flush = _inr_flush_implementation;

}