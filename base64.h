/* $Id
 *
 * drt@un.bewaff.net
 *
 * $Log: base64.h,v $
 * Revision 1.3  2001/10/08 12:51:21  drt
 * uodated emailaddress
 *
 * Revision 1.2  2000/04/20 15:06:16  drt
 * Added base64 decoding
 *
 *
 */ 

/* base64-encode.c */
int base64encode(char *dest, char *src, int l);
/* will encode l bytes from src acording to RfC1341 and write them to dest
 * dest must have space for 1/3 more bytes than l and padding to the next 
 * multiple of 3.
 * To be save you can malloc ((l/3)*4)+3 bytes.
 */

/* base64-decode.c */
int base64decode(char *dest, char *src, int l);
/* will decode l bytes from src acording to RfC1341 and write them to dest
 * dest must have space for 3/4 of the bytes in src.
 */

/* The glory details from RfC1341:

 The following is an excerpt from RFC 1341 which defines Base64 encoding.


            Network Working Group               N. Borenstein, Bellcore
            Request for Comments: 1341               N. Freed, Innosoft
                                                              June 1992
                   MIME  (Multipurpose Internet Mail Extensions):

                      Mechanisms for Specifying and Describing
                       the Format of Internet Message Bodies

                   Excerpt Defining Base64 Encoding


            5.2  Base64 Content-Transfer-Encoding

            The  Base64   Content-Transfer-Encoding   is   designed   to
            represent  arbitrary  sequences  of octets in a form that is
            not humanly readable.  The encoding and decoding  algorithms
            are simple, but the encoded data are consistently only about
            33 percent larger than the unencoded data.  This encoding is
            based on the one used in Privacy Enhanced Mail applications,
            as defined in RFC 1113.   The  base64  encoding  is  adapted
            from  RFC  1113, with one change:  base64 eliminates the "*"
            mechanism for embedded clear text.

            A 65-character subset of US-ASCII is used, enabling  6  bits
            to  be  represented per printable character. (The extra 65th
            character, "=", is used  to  signify  a  special  processing
            function.)

            NOTE:  This subset has the important  property  that  it  is
            represented   identically   in  all  versions  of  ISO  646,
            including US ASCII, and all characters  in  the  subset  are
            also  represented  identically  in  all  versions of EBCDIC.
            Other popular encodings, such as the encoding  used  by  the
            UUENCODE  utility  and the base85 encoding specified as part
            of Level 2 PostScript, do not share  these  properties,  and
            thus  do  not  fulfill the portability requirements a binary
            transport encoding for mail must meet.

            The encoding process represents 24-bit groups of input  bits
            as  output  strings of 4 encoded characters. Proceeding from
            left  to  right,  a  24-bit  input  group   is   formed   by
            concatenating  3  8-bit input groups. These 24 bits are then
            treated as 4 concatenated 6-bit groups,  each  of  which  is
            translated  into a single digit in the base64 alphabet. When
            encoding a bit stream  via  the  base64  encoding,  the  bit
            stream  must  be  presumed  to  be  ordered  with  the most-
            significant-bit first.  That is, the first bit in the stream
            will be the high-order bit in the first byte, and the eighth
            bit will be the low-order bit in the first byte, and so on.

            Each 6-bit group is used as an index into  an  array  of  64
            printable  characters. The character referenced by the index
            is placed in the output string. These characters, identified
            in  Table  1,  below,  are  selected so as to be universally
            representable,  and  the  set   excludes   characters   with
            particular  significance to SMTP (e.g., ".", "CR", "LF") and
            to the encapsulation boundaries  defined  in  this  document
            (e.g., "-").

                            Table 1: The Base64 Alphabet

               Value Encoding  Value  Encoding   Value  Encoding   Value
            Encoding
                   0 A            17 R            34 i            51 z
                   1 B            18 S            35 j            52 0
                   2 C            19 T            36 k            53 1
                   3 D            20 U            37 l            54 2
                   4 E            21 V            38 m            55 3
                   5 F            22 W            39 n            56 4
                   6 G            23 X            40 o            57 5
                   7 H            24 Y            41 p            58 6
                   8 I            25 Z            42 q            59 7
                   9 J            26 a            43 r            60 8
                  10 K            27 b            44 s            61 9
                  11 L            28 c            45 t            62 +
                  12 M            29 d            46 u            63 /
                  13 N            30 e            47 v
                  14 O            31 f            48 w         (pad) =
                  15 P            32 g            49 x
                  16 Q            33 h            50 y

            The output stream (encoded bytes)  must  be  represented  in
            lines  of  no more than 76 characters each.  All line breaks
            or other characters not found in Table 1 must be ignored  by
            decoding  software.   In  base64 data, characters other than
            those in  Table  1,  line  breaks,  and  other  white  space
            probably  indicate  a  transmission  error,  about  which  a
            warning  message  or  even  a  message  rejection  might  be
            appropriate under some circumstances.

            Special processing is performed if fewer than  24  bits  are
            available  at  the  end  of  the data being encoded.  A full
            encoding quantum is always completed at the end of  a  body.
            When  fewer  than  24  input  bits are available in an input
            group, zero bits  are  added  (on  the  right)  to  form  an
            integral number of 6-bit groups.  Output character positions
            which are not required to represent actual  input  data  are
            set  to  the  character  "=".   Since all base64 input is an
            integral number of octets,  only  the  following  cases  can
            arise:  (1)  the  final  quantum  of  encoding  input  is an
            integral multiple of  24  bits;  here,  the  final  unit  of
            encoded  output will be an integral multiple of 4 characters
            with no "=" padding, (2) the final quantum of encoding input
            is  exactly  8  bits; here, the final unit of encoded output
            will  be  two  characters  followed  by  two   "="   padding
            characters,  or  (3)  the final quantum of encoding input is
            exactly 16 bits; here, the final unit of encoded output will
            be three characters followed by one "=" padding character.

            Care must be taken to use the proper octets for line  breaks
            if base64 encoding is applied directly to text material that
            has not been converted to  canonical  form.  In  particular,
            text  line  breaks  should  be converted into CRLF sequences
            prior to base64 encoding. The important  thing  to  note  is
            that this may be done directly by the encoder rather than in
            a prior canonicalization step in some implementations.

            NOTE: There is no  need  to  worry  about  quoting  apparent
            encapsulation  boundaries  within  base64-encoded  parts  of
            multipart entities because no hyphen characters are used  in
            the base64 encoding.
	    
*/
