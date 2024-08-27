import { Text } from 'preact-i18n';
import { Link } from 'preact-router/match';
import cx from 'classnames';

import EmptyState from './EmptyState';
import style from './style.css';
import TuyaDeviceBox from '../TuyaDeviceBox';
import { connect } from 'unistore/preact';
import { Component } from 'preact';
import { RequestStatus } from '../../../../../utils/consts';

class DiscoverTab extends Component {
  async componentWillMount() {
    this.getDiscoveredDevices();
    this.getHouses();
  }

  async getHouses() {
    this.setState({
      housesGetStatus: RequestStatus.Getting
    });
    try {
      const params = {
        expand: 'rooms'
      };
      const housesWithRooms = await this.props.httpClient.get(`/api/v1/house`, params);
      this.setState({
        housesWithRooms,
        housesGetStatus: RequestStatus.Success
      });
    } catch (e) {
      this.setState({
        housesGetStatus: RequestStatus.Error
      });
    }
  }

  getDiscoveredDevices = async () => {
    this.setState({
      loading: true
    });
    try {
      const discoveredDevices = await this.props.httpClient.get('/api/v1/service/tuya/discover');
      this.setState({
        discoveredDevices,
        loading: false,
        errorLoading: false
      });
    } catch (e) {
      this.setState({
        loading: false,
        errorLoading: true
      });
    }
  };

  render(props, { loading, errorLoading, discoveredDevices, housesWithRooms }) {
    return (
      <div class="card">
        <div class="card-header">
          <h1 class="card-title">
            <Text id="integration.tuya.discover.title" />
          </h1>
          <div class="page-options d-flex">
            <button onClick={this.getDiscoveredDevices} class="btn btn-outline-primary ml-2" disabled={loading}>
              <Text id="integration.tuya.discover.scan" /> <i class="fe fe-radio" />
            </button>
          </div>
        </div>
        <div class="card-body">
          <div class="alert alert-secondary">
            <Text id="integration.tuya.discover.description" />
          </div>
          <div
            class={cx('dimmer', {
              active: loading
            })}
          >
            <div class="loader" />
            <div class={cx('dimmer-content', style.tuyaListBody)}>
              {errorLoading && (
                <p class="alert alert-warning">
                  <Text id="integration.tuya.status.notConnected" />
                  <Link href="/dashboard/integration/device/tuya/setup">
                    <Text id="integration.tuya.status.setupPageLink" />
                  </Link>
                </p>
              )}
              <div class="row">
                {discoveredDevices &&
                  discoveredDevices.map((device, index) => (
                    <TuyaDeviceBox
                      editable={!device.created_at || device.updatable}
                      alreadyCreatedButton={device.created_at && !device.updatable}
                      updateButton={device.updatable}
                      saveButton={!device.created_at}
                      device={device}
                      deviceIndex={index}
                      housesWithRooms={housesWithRooms}
                    />
                  ))}
                {!discoveredDevices || (discoveredDevices.length === 0 && <EmptyState />)}
              </div>
            </div>
          </div>
        </div>
      </div>
    );
  }
}

export default connect('httpClient', {})(DiscoverTab);
