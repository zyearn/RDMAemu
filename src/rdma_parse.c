#include "rdma_parse.h"
#include "dbg.h"

int rmda_parse_request(rdma_request_t *r) {
    u_char c, ch, *p, *m;
    enum {
        sw_start = 0,

        /* ASK */
        sw_A,
        sw_AS,
        sw_ASK,

        /* MALLOC */
        sw_M,
        sw_MA,
        sw_MAL,
        sw_MALL,
        sw_MALLO,
        sw_MALLOC,
        sw_spaces_after_malloc,
        sw_malloc_size,

        /* FREE */
        sw_F,
        sw_FR,
        sw_FRE,
        sw_FREE,
        sw_spaces_after_free,
        sw_free_ptr,

        /* GET */
        sw_G,
        sw_GE,
        sw_GET,
        sw_spaces_after_get,
        sw_get_start,

        /* SET */
        sw_S,
        sw_SE,
        sw_SET,
        sw_spaces_after_set,
        sw_set_start,

        sw_almost_done
    } state;

    state = r->state;

    for (p = r->pos; p < r->last; p++) {
        ch = *p;

        switch (state) {

        /* HTTP methods: GET, HEAD, POST */
        case sw_start:
            debug("in sw_start");
            r->request_start = p;

            if (ch == CR || ch == LF) {
                break;
            }

            if ((ch < 'A' || ch > 'Z') && ch != '_') {
                return RDMA_PARSE_INVALID_METHOD;
            }
            
            switch (ch) {
                case 'A': state = sw_A; break;
                case 'M': state = sw_M; break;
                case 'F': state = sw_F; break;
                case 'G': state = sw_G; break;
                case 'S': state = sw_S; break;
                default:
                    return RDMA_PARSE_INVALID_METHOD;
            }

            break;
        
        /* ASK */
        case sw_A:
            switch (ch) {
                case 'S': state = sw_AS; break;
                default:
                    return RDMA_PARSE_INVALID_METHOD;
            }

            break;
        
        case sw_AS:
            switch (ch) {
                case 'K': 
                    state = sw_ASK; 
                    r->request_end = p+1;
                    break;
                default:
                    return RDMA_PARSE_INVALID_METHOD;
            }

            break;

        case sw_ASK:
            switch (ch) {
                case CR: state = sw_almost_done; break;
                case LF: goto done;
                default:
                    return RDMA_PARSE_INVALID_METHOD;
            }

            break;

        /* MALLOC */
        case sw_M:
            switch (ch) {
                case 'A': state = sw_MA; break;
                default:
                    return RDMA_PARSE_INVALID_METHOD;
            }

            break;

        case sw_MA:
            switch (ch) {
                case 'L': state = sw_MAL; break;
                default:
                    return RDMA_PARSE_INVALID_METHOD;
            }

            break;
            
        case sw_MAL:
            switch (ch) {
                case 'L': state = sw_MALL; break;
                default:
                    return RDMA_PARSE_INVALID_METHOD;
            }

            break;
            
        case sw_MALL:
            switch (ch) {
                case 'O': state = sw_MALLO; break;
                default:
                    return RDMA_PARSE_INVALID_METHOD;
            }

            break;
            
        case sw_MALLO:
            switch (ch) {
                case 'C': 
                    state = sw_MALLOC;
                    r->request_end = p+1;
                    break;
                default:
                    return RDMA_PARSE_INVALID_METHOD;
            }

            break;

        case sw_MALLOC:
            switch (ch) {
                case ' ': state = sw_spaces_after_malloc; break;
                default:
                    return RDMA_PARSE_INVALID_METHOD;
            }

            break;


        case sw_spaces_after_malloc: 
            switch (ch) {
                case ' ': break;
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9': 
                    state = sw_malloc_size;
                    r->malloc_size_start = p;
                    break;
                default:
                return RDMA_PARSE_INVALID_METHOD;
            }

            break;

        case sw_malloc_size:
            switch (ch) {
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9': 
                    break;
                case CR:
                    debug("CR");
                    state = sw_almost_done;
                    r->malloc_size_end = p;
                    break;
                case LF:
                    debug("LF");
                    r->malloc_size_end = p;
                    goto done;
                default:
                return RDMA_PARSE_INVALID_METHOD;
            }

            break;

        /* SET */
        case sw_S:
            debug("in sw_S");
            switch (ch) {
                case 'E': state = sw_SE; break;
                default:
                    return RDMA_PARSE_INVALID_METHOD;
            }

            break;

        case sw_SE:
            debug("in sw_SE");
            debug("ch = %c", ch);
            switch (ch) {
                case 'T': 
                    r->request_end = p + 1;
                    state = sw_SET; 
                    break;
                default:
                    debug("ready to return");
                    return RDMA_PARSE_INVALID_METHOD;
            }
            
            break;

        case sw_SET:
            debug("in sw_SET");
            switch (ch) {
                case ' ': 
                    state = sw_spaces_after_set; 
                    break;
                default:
                    return RDMA_PARSE_INVALID_METHOD;
            }

            break;


        case sw_spaces_after_set:
            debug("in sw_spaces_after_set");
            switch (ch) {
                case ' ': break;
                default:
                    state = sw_set_start;
                    r->set_start = p;
            }

            break;

        case sw_set_start:
            debug("in sw_set_start");
            switch (ch) {
                case CR:
                    state = sw_almost_done;
                    r->set_end = p;
                    break;
                case LF: 
                    r->set_end = p;
                    goto done;
                default: break;
            }

            break;

        /* GET */
        case sw_G:
            switch (ch) {
                case 'E': state = sw_GE; break;
                default:
                    return RDMA_PARSE_INVALID_METHOD;
            }

            break;

        case sw_GE:
            switch (ch) {
                case 'T': 
                    r->request_end = p + 1;
                    state = sw_GET; 
                    break;
                default:
                    return RDMA_PARSE_INVALID_METHOD;
            }
            
            break;

        case sw_GET:
            switch (ch) {
                case ' ': 
                    state = sw_spaces_after_get; 
                    break;
                default:
                    return RDMA_PARSE_INVALID_METHOD;
            }

            break;


        case sw_spaces_after_get:
            switch (ch) {
                case ' ': break;
                default:
                    state = sw_get_start;
                    r->get_start = p;
            }

            break;

        case sw_get_start:
            switch (ch) {
                case CR:
                    state = sw_almost_done;
                    r->get_end = p;
                    break;
                case LF: 
                    r->get_end = p;
                    goto done;
                default: break;
            }

            break;

        /* end of request line */
        case sw_almost_done:
            r->end = p - 1;
            switch (ch) {
            case LF:
                goto done;
            default:
                return RDMA_PARSE_INVALID_METHOD;
            }

        default:
            return RDMA_PARSE_INVALID_METHOD;
        }
    }

    r->pos = p;
    r->state = state;

    return RDMA_AGAIN;

done:

    r->pos = p + 1;

    if (r->end == NULL) {
        r->end = p;
    }

    r->state = sw_start;

    return RDMA_OK;
}
