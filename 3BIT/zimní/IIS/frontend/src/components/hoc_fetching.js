import React from 'react';
import {Redirect} from "react-router-dom";
import axios from 'axios';

import {ROUTE_PREFIX, TOKEN_NAME} from '../const.js'

const withFetching = (url) => (Component) =>
  class WithFetching extends React.Component {
    constructor(props) {
      super(props);

      this.state = {
        data: null,
        isLoading: false,
        error: null,
      };
    }

    componentDidMount() {
        this.fetchData(url)
    }

    async fetchData(url) {
        this.setState({ isLoading: true });
        try {
            const fetchedData = await axios(url, {
                timeout: 2000,
                errorHandle: false,
                headers: {
                    token: JSON.parse(localStorage.getItem(TOKEN_NAME))['token']
                }
            })
            this.setState({
                data: fetchedData.data,
                isLoading: false
            })
        } catch (error) {
            let retCode = error.response.status
            let errorResolver = ''
            this.setState({
                isLoading: false
            })
            // zname errory
            if (retCode === 403) {
                errorResolver = 'badLogin'
            } else if (retCode === 404) {
                errorResolver = 'badPage'
            } else {
                errorResolver = error
            }
            // nastavime chybu
            this.setState({
                error: errorResolver
            })
        }
    }

    render() {
        var {error} = this.state
        if (!error) {
            return <Component { ...this.props } { ...this.state }/>;
        } else {
            if (error === 'badLogin') {
                localStorage.removeItem(TOKEN_NAME)
                return <Redirect to={`${ROUTE_PREFIX}login`}/>
            } else if (error === 'badPage') {
                return <Redirect to={`${ROUTE_PREFIX}error`}/>
            }
        }
    }
}

export default withFetching