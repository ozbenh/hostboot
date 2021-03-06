/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/usr/isteps/pbusLinkSvc.C $                                */
/*                                                                        */
/* OpenPOWER HostBoot Project                                             */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2012,2016                        */
/* [+] International Business Machines Corp.                              */
/*                                                                        */
/*                                                                        */
/* Licensed under the Apache License, Version 2.0 (the "License");        */
/* you may not use this file except in compliance with the License.       */
/* You may obtain a copy of the License at                                */
/*                                                                        */
/*     http://www.apache.org/licenses/LICENSE-2.0                         */
/*                                                                        */
/* Unless required by applicable law or agreed to in writing, software    */
/* distributed under the License is distributed on an "AS IS" BASIS,      */
/* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or        */
/* implied. See the License for the specific language governing           */
/* permissions and limitations under the License.                         */
/*                                                                        */
/* IBM_PROLOG_END_TAG                                                     */
#include    "pbusLinkSvc.H"
#include  <isteps/istep_reasoncodes.H>

namespace   EDI_EI_INITIALIZATION
{

using   namespace   TARGETING;
using   namespace   ISTEP;

PbusLinkSvc & PbusLinkSvc::getTheInstance()
{
    return Singleton<PbusLinkSvc>::instance();
}

PbusLinkSvc::PbusLinkSvc()
{
    mutex_init(&iv_mutex);
}

PbusLinkSvc::~PbusLinkSvc()
{
    mutex_destroy(&iv_mutex);
}

errlHndl_t PbusLinkSvc::getPbusConnections( TargetPairs_t & o_PbusConnections,
                                         TYPE i_busType, bool i_noDuplicate )
{
    errlHndl_t l_errl = NULL;
    TargetPairs_t * l_PbusConnections = NULL;
    o_PbusConnections.clear();

    mutex_lock(&iv_mutex);

    switch (i_busType)
    {
      case TYPE_ABUS:
    {
        if (iv_abusConnections.size() == 0)
        {
            l_errl = collectPbusConnections( TYPE_ABUS );
        }
        if (l_errl == NULL)
        {
            l_PbusConnections = i_noDuplicate ? &iv_abusUniqueConnections :
                                            &iv_abusConnections;
        }
    }
      break;
      case TYPE_OBUS:
      {
          if (iv_obusConnections.size() == 0)
          {
              l_errl = collectPbusConnections( TYPE_OBUS );
          }
          if (l_errl == NULL)
          {
              l_PbusConnections = i_noDuplicate ? &iv_obusUniqueConnections :
                                                  &iv_obusConnections;
          }
      }
      break;
      case TYPE_XBUS:
    {
        if (iv_xbusConnections.size() == 0)
        {
              l_errl = collectPbusConnections( TYPE_XBUS );
        }
        if (l_errl == NULL)
        {
            l_PbusConnections = i_noDuplicate ? &iv_xbusUniqueConnections :
                                            &iv_xbusConnections;
        }
    }
      break;
      default:
      {
          /*@
            * @errortype    ERRL_SEV_UNRECOVERABLE
            * @moduleid     MOD_EDI_EI_IO_RUN_TRAINING
            * @reasoncode   RC_INVALID_TARGET_TYPE
            * @userdata1    bus type
            * @custdesc     Platform generated error. See User Data.
            * @devdesc      Unexpected bus target type
            */
            l_errl = new ERRORLOG::ErrlEntry(
                            ERRORLOG::ERRL_SEV_UNRECOVERABLE,
                            MOD_EDI_EI_IO_RUN_TRAINING,
                            RC_INVALID_TARGET_TYPE,
                            i_busType );
      }
      break;
    }

    if (l_errl == NULL)
    {
        o_PbusConnections.insert( (*l_PbusConnections).begin(),
                                              (*l_PbusConnections).end() );
    }

    mutex_unlock(&iv_mutex);

    return l_errl;
}


errlHndl_t PbusLinkSvc::collectPbusConnections( TYPE i_busType )
{
    errlHndl_t l_errl = NULL;

    // Get all functional i_busType chiplets
    TARGETING::TargetHandleList l_busTargetList;
    getAllChiplets(l_busTargetList, i_busType);
    TRACFCOMP( TARGETING::g_trac_targeting,
             "PbusLinkSvc::collectPbusConnections - getAllChiplets(%d type) "
             "returned %d entries", i_busType, l_busTargetList.size());

    // select the appropriate maps to work with
    TargetPairs_t * l_pPbusConnections;
    TargetPairs_t * l_pPbusUniqueConnections;
    switch (i_busType)
    {
      case TYPE_ABUS:
      {
          l_pPbusConnections = &iv_abusConnections;
          l_pPbusUniqueConnections = &iv_abusUniqueConnections;
          break;
      }
      case TYPE_OBUS:
      {
          l_pPbusConnections = &iv_obusConnections;
          l_pPbusUniqueConnections = &iv_obusUniqueConnections;
          break;
      }
      case TYPE_XBUS:
      default:
      {
          l_pPbusConnections = &iv_xbusConnections;
          l_pPbusUniqueConnections = &iv_xbusUniqueConnections;
          break;
      }
    }

    // Collect all functional i_busType pbus connections
    for (const auto & l_pTarget: l_busTargetList)
    {
        // exit for loop if error already encountered
        if (NULL != l_errl)
    {
            break;
        }

        // get other endpoint target
        TRACFCOMP( TARGETING::g_trac_targeting,
                 "Get other endpoint target for target HUID %.8X",
                 TARGETING::get_huid(l_pTarget) );

        const TARGETING::Target * l_dstTgt =
                    l_pTarget->getAttr<ATTR_PEER_TARGET>();

        TRACFCOMP( TARGETING::g_trac_targeting,
                 "Other endpoint target HUID %.8X",
                 TARGETING::get_huid(l_dstTgt) );

        // connection is existing, not to itself and is a real target
        if ((l_dstTgt != NULL) && (l_dstTgt != l_pTarget))
        {
            TYPE l_dstType = l_dstTgt->getAttr<ATTR_TYPE>();
            if (l_dstType != i_busType)
            {
                TRACFCOMP(TARGETING::g_trac_targeting,
                    "Both endpoints' bus type mismatch; "
                         "target HUID %.8X: dest HUID %.8X",
                    TARGETING::get_huid(l_pTarget),
                    TARGETING::get_huid(l_dstTgt));

                // Mixed bus type connection
                /*@
                 * @errortype    ERRL_SEV_UNRECOVERABLE
                 * @moduleid     MOD_EDI_EI_IO_RUN_TRAINING
                 * @reasoncode   RC_MIXED_PBUS_CONNECTION
                 * @userdata1    Endpoint1 bus type
                 * @userdata2    Endpoint2 bus type
                 * @custdesc      Platform generated error. See User Data.
                 * @devdesc      Mixed bus connection of two types
                 */
                l_errl = new ERRORLOG::ErrlEntry(
                                         ERRORLOG::ERRL_SEV_UNRECOVERABLE,
                                         MOD_EDI_EI_IO_RUN_TRAINING,
                                         RC_MIXED_PBUS_CONNECTION,
                                         i_busType,
                                         l_dstType );
                break;
            }

            // Get the chip parents of endpoints
            const TARGETING::Target * l_endp1Parent = getParentChip(l_pTarget);
            const TARGETING::Target * l_endp2Parent = getParentChip(l_dstTgt);

            if (l_endp1Parent == l_endp2Parent)
            {
                TRACFCOMP(TARGETING::g_trac_targeting,
                    "Both endpoints from same chip; "
                    "target HUID %.8X dest HUID %.8X",
                    TARGETING::get_huid(l_pTarget),
                    TARGETING::get_huid(l_dstTgt));

                // connection of same chip
                /*@
                 * @errortype    ERRL_SEV_UNRECOVERABLE
                 * @moduleid     MOD_EDI_EI_IO_RUN_TRAINING
                 * @reasoncode   RC_SAME_CHIP_PBUS_CONNECTION
                 * @custdesc     Platform generated error.
                 * @devdesc      Both endpoint connections of same chip
                 */
                l_errl = new ERRORLOG::ErrlEntry(
                                     ERRORLOG::ERRL_SEV_UNRECOVERABLE,
                                     MOD_EDI_EI_IO_RUN_TRAINING,
                                     RC_SAME_CHIP_PBUS_CONNECTION );
                break;
            }


            for (const auto l_dstTarget: l_busTargetList)
            {
                // l_dstTgt is functional
                if (l_dstTgt == l_dstTarget)
                {
                    // save the pair if not yet done so
                    (*l_pPbusConnections)[l_pTarget] = l_dstTgt;
                    (*l_pPbusUniqueConnections)[l_pTarget] = l_dstTgt;
                    break;
                }
            }
        }
    } // for l_bus_iter

    // Validate pbus connections are valid and strike out
    // duplicates for the Unique connection map
    TargetPairs_t::iterator l_jtr;

    for (const auto l_PbusConnection : (*l_pPbusUniqueConnections))
    {
        // exit for loop if error already encountered
        if (NULL != l_errl)
        {
            break;
        }

        const TARGETING::Target *l_ptr1 = l_PbusConnection.first;
        const TARGETING::Target *l_ptr2 = l_PbusConnection.second;
        l_jtr = l_pPbusUniqueConnections->find(l_ptr2);
        if ( (l_jtr == l_pPbusUniqueConnections->end()) ||
             (l_jtr->second != l_ptr1) )
        {
            // Connection is conflicting, e.g.
            //   endp1 -> endp2 but endp2 -> endp3.
            //   endp1 -> endp2 but endp2 -> endp2 (itself) or not existing
            EntityPath l_path;
            l_path = l_ptr2->getAttr<ATTR_PHYS_PATH>();
            char *l_pathString = l_path.toString();
            TARG_ERR( "First endpoint's PEER_TARGET is %s", l_pathString );
            free (l_pathString);
            if (l_jtr != l_pPbusUniqueConnections->end())
            {
                l_path = l_jtr->second->getAttr<ATTR_PHYS_PATH>();
                l_pathString = l_path.toString();
                TARG_ERR("Second endpoint's PEER_TARGET is %s", l_pathString);
                free (l_pathString);
            }
            else
            {
                if (l_ptr2)
                {
                    l_path = l_ptr2->getAttr<ATTR_PHYS_PATH>();
                    l_pathString = l_path.toString();
                    TARG_ERR("Second endpoint's PEER_TARGET is itself, %s",
                                                                l_pathString);
                    free (l_pathString);
                }
                else
                {
                    TARG_ERR("Second endpoint's PEER_TARGET is not existing");
                }
            }

            /*@
             * @errortype    ERRL_SEV_UNRECOVERABLE
             * @moduleid     MOD_EDI_EI_IO_RUN_TRAINING
             * @reasoncode   RC_CONFLICT_PBUS_CONNECTION
             * @userdata1    Bus endpoint target pointer1
             * @userdata2    Bus endpoint target pointer2
             * @custdesc     Platform generated error. See User Data.
             * @devdesc      Connection is conflicting.
             *               endp1 -> endp2 but endp2 -> endp3.
             *               endp1 -> endp2 but
             *               endp2 -> endp2 (itself) or not existing
             */
            l_errl = new ERRORLOG::ErrlEntry(ERRORLOG::ERRL_SEV_UNRECOVERABLE,
                                         MOD_EDI_EI_IO_RUN_TRAINING,
                                         RC_CONFLICT_PBUS_CONNECTION,
                                         reinterpret_cast<uint64_t>(l_ptr1),
                                         reinterpret_cast<uint64_t>(l_ptr2));
            break;
        }
        else
        {
          l_pPbusUniqueConnections->erase(l_jtr);
        }
    }

    return l_errl;
}

}
